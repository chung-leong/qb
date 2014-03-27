--TEST--
Use case: Quaternion Julia Set Raytracer (multicore)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

/**
 * QuaternionJulia.pbk
 *
 * This Pixel Bender port is based on the Quaternion Julia Set raytracer 
 * Cg fragment shader by Keenan Crane (kcrane@uiuc.edu).
 * 
 * Please visit his website for many more interesting articles:
 * http://graphics.cs.uiuc.edu/~kcrane
 * 
 * There is also a good write-up explaining the process at:
 * http://www.devmaster.net/forums/showthread.php?t=4448
 *
 * This version was created by Tom Beddard. 
 * See my site for more generative graphics experiments:
 * http://www.subblue.com
 *
 *
 * Overview
 * ===================
 * For each pixel a ray is fired into the scene starting from a bounding sphere. As the
 * ray steps forward a distance estimate is dertermined to the nearest part of the fractal
 * and this is used as the next step size. If a ray gets within a minimum distance of the
 * isosurface it is considered a 'hit'. Shading is performed by aproximating the gradient 
 * of the distance function to create a surface normal.
 * 
 * A more complete description of the sphere tracing method can be found in John Hart's paper,
 * "Ray Tracing Deterministic 3-D Fractals":
 * http://graphics.cs.uiuc.edu/~jch/papers/rtqjs.pdf
 * 
 * 
 * Additions this Pixel Bender version:
 *  - Added an antialiasing option to do 4x super sampling per pixel
 *  - Added a crude ambient occlusion shading factor which is based on the fact that the
 *    step size is proportional to the nearest part of the isosurface, so for some cases
 *    the greater the number of steps required for a hit (due to smaller step sizes) then 
 *    the more occluded the final point. Darkening the shading of this point helps define
 *    smaller details.
 *    To get the classic 'white clay' ambient occlusion look:
 *      - increase the ambient light to 1.0
 *      - reduce the specularity and colorSpread to 0
 * 
 * 
 * License
 * ===================
 * This filter is based upon Keenan Crane's original Cg shader which was released under the 
 * BSD license listed below. This version is released under the same license.
 *
 * Copyright (c) 1984-2006 Keenan Crane.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Keenan Crane.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
       * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

class QuaternionJulia {

	/** @var bool */
	public $antialiasing = false;

	/** @var float32 */
	public $ambientLight = 0.64;

	/** @var float32 */
	public $ambientOcclusion = 1.3;
    
	/** @var float32 */
	public $shadows = 0.0;

	/** @var float32[4] */
	public $mu = array(-0.04, 0, 0.72, 0);

	/** @var float32[4] */
	public $camera = array(-30, 0, -20, 3);

	/** @var float32[3] */
	public $light = array(2, 2, 2);
	
	/** @var float32[3] */
	public $background = array(0.3, 0.33, 0.35);
	
	/** @var float32 */
	public $backgroundTransparency = 1.0;

	/** @var float32[3] */
	public $color = array(0.63, 0.08, 0.0);
	
	/** @var float32 */
	public $colorSpread = 0.13;

	/** @var float32 */
	public $specularity = 0.5;
    
	/** @var float32 */
	public $specularExponent = 7.0;
	
	/** @var int32 */
	public $maxIterations = 8;

	/** @var float32 */
	protected $width;

	/** @var float32 */
	protected $height;

	/** @var float32 */
	protected $aspectRatio;
	
	/** @var float32 */
	protected $sampleStep;
	
	/** @var float32 */
	protected $sampleContribution;

	/** @var float32[3] */
	protected $eye;

	/** @var float32[3] */
	protected $lightSource;

	/** @var float32[3][3] */
	protected $viewRotation;
	

	// Some constants used in the ray tracing process.  (These constants
	// were determined through trial and error and are not by any means
	// optimal.)

	// Radius of a bounding sphere for the set used to accelerate intersection
	const BOUNDING_RADIUS_2 = 3.0;

	// Any series whose points' magnitude exceed this threshold are considered divergent
	const ESCAPE_THRESHOLD = 1e1;

	// Delta is used in the finite difference approximation of the gradient (to determine normals)
	const DEL = 1e-4;

	// The distance at which the ray has to be within the isosurface for a 'hit'
	const EPSILON = 1e-3;


	/** 
	 * Returns the product of quaternions q1 and q2.
	 * Note that quaternion multiplication is NOT commutative (i.e., q1 ** q2 != q2 ** q1 ).
	 *
	 * @engine qb
	 *
	 * @param float32[4]	$q1
	 * @param float32[4]	$q2
	 *
	 * @local float32[4]	$r
	 *
	 * @return float32[4]
	 */
	 protected function quatMult($q1, $q2)
	 {
		$r[0] = $q1[0]*$q2[0] - $q1[1]*$q2[1] - $q1[2]*$q2[2] - $q1[3]*$q2[3];
		$r[1] = $q1[0]*$q2[1] + $q1[1]*$q2[0] + $q1[2]*$q2[3] - $q1[3]*$q2[2];
		$r[2] = $q1[0]*$q2[2] - $q1[1]*$q2[3] + $q1[2]*$q2[0] - $q1[3]*$q2[1];
		$r[3] = $q1[0]*$q2[3] + $q1[1]*$q2[2] - $q1[2]*$q2[1] - $q1[3]*$q2[0];
		return $r;
	}
	
	
	/**
	 * Returns the square of quaternion q.  This function is a special (optimized)
	 * case of quatMult().
	 *
	 * @engine qb
	 *
	 * @param float32[4]	$q
	 *
	 * @local float32[4]	$r
	 *
	 * @return float32[4]
	 */
	protected function quatSq($q)
	{
		$r[0] = $q[0]*$q[0] - $q[1]*$q[1] - $q[2]*$q[2] - $q[3]*$q[3];
		$r[1] = 2.0*$q[0]*$q[1];
		$r[2] = 2.0*$q[0]*$q[2];
		$r[3] = 2.0*$q[0]*$q[3];
		return $r;
	}
    
	
	/** 
	 * Iterates the quaternion q for the purposes of intersection.  This function also
	 * produces an estimate of the derivative at q, which is required for the distance
	 * estimate.  The quaternion c is the parameter specifying the Julia set, and the
	 * integer maxIterations is the maximum number of iterations used to determine
	 * whether a point is in the set or not.
	 *
	 * To estimate membership in the set, we recursively evaluate
	 * 
	 * q = q*q + c
	 *
	 * until q has a magnitude greater than the threshold value (i.e., it probably
	 * diverges) or we've reached the maximum number of allowable iterations (i.e.,
	 * it probably converges).  More iterations reveal greater detail in the set.
	 *
	 * To estimate the derivative at q, we recursively evaluate
	 *
	 * q' = 2*q*q'
	 *
	 * concurrently with the evaluation of q.
	 *
	 * @engine qb
	 *
	 * @param float32[4]	$q
	 * @param float32[4]	$qp
	 * @param float32[4]	$c
	 * @param int32			$maxIterations
	 *
	 * @local int32			$i
	 *
	 * @return void
	 */
	protected function iterateIntersect(&$q, &$qp, $c, $maxIterations) {
		for ($i = 0; $i < $maxIterations; $i++) {
			$qp = 2.0 * $this->quatMult($q, $qp);
			$q = $this->quatSq($q) + $c;

			if (dot($q, $q) > self::ESCAPE_THRESHOLD) break;
		}
	}
	
	
	/**
	 * Create a shading normal for the current point.  We use an approximate normal of
	 * the isosurface of the potential function, though there are other ways to
	 * generate a normal (e.g., from an isosurface of the potential function).
	 *
	 * @engine qb
	 *
	 * @param float32[3]	$p
	 * @param float32[4]	$c
	 *
	 * @local float32[3]	$N
	 * @local float32[4]	$qP
	 * @local float32[3]	$grad
	 * @local float32[4]	$(gx1|gx2|gy1|gy2|gz1|gz2)
	 * @local int32			$i
	 *
	 * @return float32[3]
	 */
	 protected function normEstimate($p, $c)
	 {
		$qP = array($p[0], $p[1], $p[2], 0.0);

		$gx1 = $qP - array(self::DEL, 0.0, 0.0, 0.0);
		$gx2 = $qP + array(self::DEL, 0.0, 0.0, 0.0);
		$gy1 = $qP - array(0.0, self::DEL, 0.0, 0.0);
		$gy2 = $qP + array(0.0, self::DEL, 0.0, 0.0);
		$gz1 = $qP - array(0.0, 0.0, self::DEL, 0.0);
		$gz2 = $qP + array(0.0, 0.0, self::DEL, 0.0);

		for ($i = 0; $i < $this->maxIterations; $i++) {
			$gx1 = $this->quatSq($gx1) + $c;
			$gx2 = $this->quatSq($gx2) + $c;
			$gy1 = $this->quatSq($gy1) + $c;
			$gy2 = $this->quatSq($gy2) + $c;
			$gz1 = $this->quatSq($gz1) + $c;
			$gz2 = $this->quatSq($gz2) + $c;
		}

		$grad[0] = length($gx2) - length($gx1);
		$grad[1] = length($gy2) - length($gy1);
		$grad[2] = length($gz2) - length($gz1);

		$N = normalize($grad);
		return $N;
	}
	
	
	/**
	 * Finds the intersection of a ray with origin rO and direction rD with the
	 * quaternion Julia set specified by quaternion constant c.  The intersection
	 * is found using iterative sphere tracing, which takes a conservative step
	 * along the ray at each iteration by estimating the minimum distance between
	 * the current ray origin and the closest point in the Julia set.  The
	 * parameter maxIterations is passed on to iterateIntersect() which determines
	 * whether the current ray origin is in (or near) the set.
	 *
	 * @engine qb
	 *
	 * @param float32[3]	$rO
	 * @param float32[3]	$rD
	 * @param float32[4]	$c
	 *
	 * @local float32[2]	$dist
	 * @local float32[4]	$z
	 * @local float32[4]	$zp
	 * @local float32		$normZ
	 * @local int32			$n
	 *
	 * @return float32[2]
	 */
	protected function intersectQJulia(&$rO, &$rD, &$c)
	{
		// The (approximate) distance between the first point along the ray within
		// epsilon of some point in the Julia set, or the last point to be tested if
		// there was no intersection.
		while ($n < 150) {
			// Iterate on the point at the current ray origin. We want to know if this 
			// point belongs to the set.
			$z = array($rO[0], $rO[1], $rO[2], 0.0);
			
			// Start the derivative at real 1. The derivative is needed to get a lower 
			// bound on the distance to the set.
			$zp = array(1.0, 0.0, 0.0, 0.0);
			
			// Iterate this point until we can guess if the sequence diverges or converges.
			$this->iterateIntersect($z, $zp, $c, $this->maxIterations);
			
			// Find a lower bound on the distance to the Julia set and step this far along the ray.
			$normZ = length($z);
			$dist[0] = 0.5 * $normZ * log($normZ) / length($zp);  // lower bound on distance to surface
			
			$rO += $rD * $dist[0]; // (step)
			
			// Intersection testing finishes if we're close enough to the surface
			// (i.e., we're inside the epsilon isosurface of the distance estimator function) 
			// or have left the bounding sphere.
			if ($dist[0] < self::EPSILON || dot($rO, $rO) > self::BOUNDING_RADIUS_2) break;
            
			$n++;
		}
        
        // Store a crude ambient occlusion factor to emphasise the shading.
        // The number of steps (n) taken to gain a hit is related to the step size, 
        // which is the distance from the the current point to the nearest part of the iso-surface,
        // so there is a casual relationship with the number of steps and how occluded the point is.
        $dist[1] = 1.0 - clamp(($n / 150.0) * $this->ambientOcclusion, 0.0, 1.0);
		return $dist;
	}
	
	
	/**
	 * Computes the direct illumination for point pt with normal N due to
	 * a point light at light and a viewer at eye.
	 *
	 * @engine qb
	 *
	 * @param float32[3]	$light
	 * @param float32[3]	$eye
	 * @param float32[3]	$pt
	 * @param float32[3]	$N
	 * @param float32		$ao
	 *
	 * @local float32[3]	$diffuse
	 * @local float32[3]	$L
	 * @local float32[3]	$E
	 * @local float32[3]	$R
	 * @local float32		$NdotL
	 *
	 * @return float32[3]
	 */
	protected function Phong($light, $eye, $pt, $N, $ao) 
	{
		$diffuse = $this->color;		// Base colour
		$L = normalize($light - $pt);  	// find the vector to the light
		$E = normalize($eye - $pt);  	// find the vector to the eye
		$NdotL = dot($N, $L);			// find the cosine of the angle between light and normal
		$R = $L - 2.0 * $NdotL * $N;	// find the reflected vector
		
		// Add some of the normal to the color to make it more interesting
		$diffuse += abs($N) * $this->colorSpread;
        
		// compute the illumination using the Phong equation
		$diffuse = ($diffuse * max($NdotL, $this->ambientLight) + $this->specularity * pow(max(dot($E,$R), 0.0), $this->specularExponent)) * $ao;
        
        return $diffuse;
	}
	
	/**	
	 * Finds the intersection of a ray with a sphere with statically
	 * defined radius BOUNDING_RADIUS centered around the origin.  This
	 * sphere serves as a bounding volume for the Julia set.
	 *
	 * @engine qb
	 *
	 * @param float32[3]	$rO
	 * @param float32[3]	$rD
	 *
	 * @local float32		$B
	 * @local float32		$C
	 * @local float32		$d
	 * @local float32		$t0
	 * @local float32		$t1
	 * @local float32		$t
	 *
	 * @return float32[3]
	 */
	protected function intersectSphere($rO, $rD) 
	{
		$B = 2.0 * dot($rO, $rD);
		$C = dot($rO, $rO) - self::BOUNDING_RADIUS_2;
		$d = sqrt($B*$B - 4.0 * $C) ;
		$t0 = (-$B + $d) * 0.5;
		$t1 = (-$B - $d) * 0.5;
		$t = min($t0, $t1);
		$rO += $t * $rD;
		
		return $rO;
	}
	
	/**
	 * Define the ray direction from the pixel coordinates
	 *
	 * @engine qb
	 *
	 * @param float32[2]	$p
	 *
	 * @local float32[3]	$direction
	 *
	 * @return float32[3]
	 */
	protected function rayDirection($p) 
	{
		$direction[0] = 2.0 * $this->aspectRatio * $p[0] / $this->width - $this->aspectRatio;
		$direction[1] = -2.0 * $p[1] / $this->height + 1.0;
		$direction[2] = -2.0;
		
		return vm_mult($direction, $this->viewRotation);
	}
	
	/**
	 * Render the current pixel point
	 *
	 * -move the ray origin forward onto a bounding sphere surrounding the Julia set
	 * -test the new ray for the nearest intersection with the Julia set
	 * -if the ray does include a point in the set:
	 *		-estimate the gradient of the potential function to get a "normal"
	 *		-use the normal and other information to perform Phong shading
	 *		-cast a shadow ray from the point of intersection to the light
	 *		-if the shadow ray hits something, modify the Phong shaded color to represent shadow
	 * -return the shaded color if there was a hit and the background color otherwise
	 *
	 * @engine qb
	 *
	 * @param float32[2]	$p
	 *
	 * @local float32[3]	$color
	 * @local float32		$alpha
	 * @local float32[3]	$rO
	 * @local float32[3]	$rD
	 * @local float32[2]	$dist
	 * @local float32[3]	$N
	 * @local float32[3]	$L
	 *
	 * @return float32[4]
	 */
	 protected function renderPoint($p) 
	 {
		$color = $this->background;
		$alpha = $this->backgroundTransparency;
		$rO = $this->eye;
		$rD = $this->rayDirection($p);
		
		// First, intersect the original ray with a sphere bounding the set, and
		// move the origin to the point of intersection.  This prevents an
		// unnecessarily large number of steps from being taken when looking for
		// intersection with the Julia set.
		$rD = normalize($rD);  //the ray direction is interpolated and may need to be normalized
		$rO = $this->intersectSphere($rO, $rD);
		
		// Next, try to find a point along the ray which intersects the Julia set.
		// (More details are given in the routine itself.)
		if (dot($rO, $rO) < self::BOUNDING_RADIUS_2 + 0.01) {
			$dist = $this->intersectQJulia($rO, $rD, $this->mu);
			// We say that we found an intersection if our estimate of the distance to
			// the set is smaller than some small value epsilon.  In this case we want
			// to do some shading / coloring.
			
			if ($dist[0] < self::EPSILON) {
				// Determine a "surface normal" which we'll use for lighting calculations.
				$N = $this->normEstimate($rO, $this->mu);
				
				// Compute the Phong illumination at the point of intersection.
				$color = $this->Phong($this->lightSource, $rD, $rO, $N, $dist[1]);
				$alpha = 1.0;  // (make this fragment opaque)
				
				// If the shadow flag is on, determine if this point is in shadow
				if ($this->shadows > 0.0) {
					// The shadow ray will start at the intersection point and go
					// towards the point light.  We initially move the ray origin
					// a little bit along this direction so that we don't mistakenly
					// find an intersection with the same point again.
					
					$L = normalize($this->lightSource - $rO);
					$rO += $N * self::EPSILON * 2.0;
					$dist = $this->intersectQJulia($rO, $L, $this->mu);
					
					// Again, if our estimate of the distance to the set is small, we say
					// that there was a hit.  In this case it means that the point is in
					// shadow and should be given darker shading.
					if ($dist[0] < self::EPSILON) {
						// (darkening the shaded value is not really correct, but looks good)
						$color *= 1.0 - $this->shadows;
					}
				}
			}
		}
			
		return array($color[0], $color[1], $color[2], $alpha);
	}
	
	/**
	 * The main loop
	 *
	 * @engine qb
	 *
	 * @param image			$dst
	 *
	 * @local uint32		$width
	 * @local uint32		$height
	 * @local uint32		$x
	 * @local uint32		$y
	 * @local float32		$i
	 * @local float32		$j
	 * @local float32[4]	$c
	 * @local float32		$(c1|c2|c3)
	 * @local float32		$(s1|s2|s3)
	 * @local float32[3][3]	$viewRotationX
	 * @local float32[3][3]	$viewRotationY
	 * @local float32[3][3]	$viewRotationZ
	 * @local float32[2]	$coord
	 *
	 * @return void
	 */
	public function generate(&$dst)
	{
		$height = count($dst);
		$width = count($dst[0]);
		
		$this->width = $width;
		$this->height = $height;
		$this->aspectRatio = $this->width / $this->height;
		
		// Camera orientation
		$c1 = cos(deg2rad(-$this->camera[0]));
		$s1 = sin(deg2rad(-$this->camera[0]));
		$viewRotationY = array($c1, 0, $s1, 0, 1, 0, -$s1, 0, $c1);
		
		$c2 = cos(deg2rad(-$this->camera[1]));
		$s2 = sin(deg2rad(-$this->camera[1]));
        $viewRotationZ = array($c2, -$s2, 0, $s2, $c2, 0, 0, 0, 1);
        
        $c3 = cos(deg2rad(-$this->camera[2]));
		$s3 = sin(deg2rad(-$this->camera[2]));
        $viewRotationX = array(1, 0, 0, 0, $c3, -$s3, 0, $s3, $c3);
        
		$this->viewRotation = mm_mult($viewRotationZ, mm_mult($viewRotationY, $viewRotationX));
        
		$this->eye = vm_mult(array(0, 0, $this->camera[3]), $this->viewRotation);
        $this->lightSource = vm_mult($this->light, $this->viewRotation);
        
        $y = fork($height);
        	
        $coord[1] = $y + 0.5;
        
        for ($x = 0, $coord[0] = 0.5; $x < $width; $x++, $coord[0]++) {
	        if ($this->antialiasing) {
				// Average detailSuperSample^2 points per pixel
				$c = array(0, 0, 0, 0);
	        	for ($i = 0.0; $i < 1.0; $i += 0.5)
					for ($j = 0.0; $j < 1.0; $j += 0.5)
						$c += 0.25 * $this->renderPoint(array($coord[0] + $i, $coord[1] + $j));
	        } else {
				$c = $this->renderPoint($coord);
	        }
			// Return the final color which is still the background color if we didn't hit anything.
	        $dst[$y][$x] = $c;
	    }
	}	

}

// so the script wouldn't slow to a grind if xdebug is active
ini_set("qb.allow_debugger_inspection", 0);
ini_set("qb.column_major_matrix", 1);

$folder = dirname(__FILE__);
$output = imagecreatetruecolor(600, 400);
$correct_path = "$folder/output/quaternion-julia.correct.png";
$incorrect_path = "$folder/output/quaternion-julia.fork.incorrect.png";

$rayTracer = new QuaternionJulia;
$rayTracer->antialiasing = false;
$rayTracer->generate($output);

ob_start();
imagesavealpha($output, true);
imagepng($output);
$output_png = ob_get_clean();

/**
 * @engine qb
 *
 * @param image	$img2;
 * @param image	$img1;
 * @return float32
 */
function _image_diff($img1, $img2) {
	$img2 -= $img1;
	$img2 *= $img2;
	return sqrt(array_sum($img2));
}

if(file_exists($correct_path)) {
	$correct_md5 = md5_file($correct_path);
	$output_md5 = md5($output_png);
	if($correct_md5 == $output_md5) {
		// exact match
		$match = true;
	} else {
		$correct_output = imagecreatefrompng($correct_path);
		$diff = _image_diff($output, $correct_output);
		if($diff < 3) {
			// the output is different ever so slightly
			$match = true;
		} else {
			$match = false;
		}
	}
	if($match) {
		echo "CORRECT\n";
		if(file_exists($incorrect_path)) {
			unlink($incorrect_path);
		}
	} else {
		echo "INCORRECT (diff = $diff)\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
	echo "CORRECT\n";
}


?>
--EXPECT--
CORRECT
