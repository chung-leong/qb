--TEST--
Use case: Mandelbulb julia fractal (multicore)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

/**
 * MandelbulbQuick.pbk
 * Last update: 14 December 2009
 *
 * Changelog:
 *		1.0		- Initial release
 *		1.0.1	- Fixed a missing asymmetry thanks to Chris King (http://www.dhushara.com)
 *				- Refinements in the colouring
 *      1.0.2   - Added radiolaria option for a funky hair-like effect
 * 				- Incorporated the scalar derivative method as described here:
 *			    - http://www.fractalforums.com/mandelbulb-implementation/realtime-renderingoptimisations/
 *		1.0.3	- Created a quick version of the script as using a boolean flag to determine
 *				  which distance estimation method created long compilation times.
 * 		1.0.4 	- Fixed issue with older graphic cards and the specular highlights
 *
 * 
 * Copyright (c) 2009 Tom Beddard
 * http://www.subblue.com
 *
 * For more Flash and PixelBender based generative graphics experiments see:
 * http://www.subblue.com/blog
 *
 * Licensed under the MIT License:
 * http://www.opensource.org/licenses/mit-license.php
 *
 *
 * Credits and references
 * ======================
 * For the story behind the 3D Mandelbrot see the following page:
 * http://www.skytopia.com/project/fractal/mandelbulb.html
 *
 * The original forum disussion with many implementation details can be found here:
 * http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 *
 * This implementation references the 4D Quaternion GPU Raytracer by Keenan Crane:
 * http://www.devmaster.net/forums/showthread.php?t=4448
 *
 * and the NVIDIA CUDA/OptiX implementation by cbuchner1:
 * http://forums.nvidia.com/index.php?showtopic=150985
 *
 */

class Mandelbulb {

	/** @var int	 		Super sampling quality. Number of samples squared per pixel. */
	public $antialiasing = 1;
	
	/** @var bool	 		Enable phong shading. */
	public $phong = true;

	/** @var bool	 		Enable Julia set version. */
	public $julia = false;

	/** @var bool	 		Enable radiolaria style. */
	public $radiolaria = false;

	/** @var float 			Tweak the radiolaria effect. */
	public $radiolariaFactor = 0.0;

	/** @var float	 		Enable ray traced shadows. */
	public $shadows = 0.0;

	/** @var float	 		Enable fake ambient occlusion factor based on the orbit trap. */
	public $ambientOcclusion = 0.8;

	/** @var float	 		Emphasise the structure edges based on the number of steps it takes to reach a point in the fractal. */
	public $ambientOcclusionEmphasis = 0.58;

	/** @var float	 		Sets the bounding sphere radius to help accelerate the raytracing. */
	public $bounding = 2.5;

	/** @var float	 		Sets the bailout value for the fractal calculation. Lower values give smoother less detailed results. */
	public $bailout = 4.0;

	/** @var float	 		The power of the fractal. */
	public $power = 8.0;

	/** @var float[3] 		The c constant for Julia set fractals */
	public $julia_c = array(1, 0, 0);

	/** @var float[x,y,z]		Camera position. */
	public $cameraPosition = array(0, -2.6, 0);

	/** @var float[x,y,z]	Fine tune position. */
	public $cameraPositionFine = array(0, 0.0, 0.0);

	/** @var float[x,y,z] 	Pointing angle in each axis of the camera. */
	public $cameraRotation = array(0, 0, -90);

	/** @var float	 		Zoom the camera view. */
	public $cameraZoom = 0.0;

	/** @var float[x,y,z]	Position of point light. */
	public $light = array(38, -42, 38);

	/** @var float[r,g,b]	Background colour. */
	public $colorBackground = array(0.0, 0, 0);

	/** @var float	 		Background transparency. */
	public $colorBackgroundTransparency = 1.0;
    
	/** @var float[r,g,b]	Diffuse colour. */
	public $colorDiffuse = array(0.0, 0.85, 0.99);

	/** @var float[r,g,b]	Ambient light colour. */
	public $colorAmbient = array(0.67, 0.85, 1.0);
	
	/** @var float	 		Ambient light intensity. */
	public $colorAmbientIntensity = 0.4;

	/** @var float[r,g,b]	Light colour. */
	public $colorLight = array(0.48, 0.59, 0.66);

	/** @var float	 		Vary the colour based on the normal direction. */
	public $colorSpread = 0.2;

	/** @var float	 		Rim light factor. */
	public $rimLight = 0.0;

	/** @var float	 		Phong specularity */
	public $specularity = 0.66;

	/** @var float	 		Phong shininess */
	public $specularExponent = 15.0;

	/** @var float[x,y,z]	Rotate the Mandelbulb in each axis. */
	public $rotation = array(0, 36, 39.6);

	/** @var int		 	More iterations reveal more detail in the fractal surface but takes longer to calculate. */
	public $maxIterations = 6;

	/** @var int		 	The maximum number of steps a ray should take. */
	public $stepLimit = 110;

	/** @var float		 	Scale the epsilon step distance. Smaller values are slower but will generate smoother results for thin areas. */
	public $epsilonScale = 1.0;

	/** @var int */
	protected $width;
	
	/** @var int */
	protected $height;

	/** @var float */
	protected $aspectRatio;

	/** @var float */
	protected $sampleStep;

	/** @var float */
	protected $sampleContribution;
	
	/** @var float */
	protected $pixelScale;

	/** @var float[3] */
	protected $eye;

	/** @var float[3][3] */
	protected $viewRotation;

	/** @var float[3][3] */
	protected $objRotation;
	
	/** @var float */
	protected $cameraZoomExponential;
	
	const MIN_EPSILON = 3e-7;
	
	/** 
	 * Scalar derivative approach by Enforcer:
	 * http://www.fractalforums.com/mandelbulb-implementation/realtime-renderingoptimisations/
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]	$z
	 * @param float			$zr0
	 * @param float			$dr
	 *
	 * @local float			$(zo0|zi0|zr|zo|zi|czo)
	 *
	 * @return void
	 */
	protected function powN(&$z, $zr0, &$dr) 
	{
		$zo0 = asin($z->z / $zr0);
		$zi0 = atan2($z->y, $z->x);
		$zr = pow($zr0, $this->power - 1.0);
		$zo = $zo0 * $this->power;
		$zi = $zi0 * $this->power;
		$czo = cos($zo);

		$dr = $zr * $dr * $this->power + 1.0;
		$zr *= $zr0;

		$z = $zr * array($czo*cos($zi), $czo*sin($zi), sin($zo));
	}

	/**
	 * The fractal calculation
	 *
	 * Calculate the closest distance to the fractal boundary and use this
	 * distance as the size of the step to take in the ray marching.
	 *
	 * Fractal formula:
	 *	  z' = z^p + c
	 *
	 * For each iteration we also calculate the derivative so we can estimate
	 * the distance to the nearest point in the fractal set, which then sets the
	 * maxiumum step we can move the ray forward before having to repeat the calculation.
	 *
	 *	 dz' = p * z^(p-1)
	 *
	 * The distance estimation is then calculated with:
	 *
	 *   0.5 * |z| * log(|z|) / |dz|
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]	$z0
	 * @param float			$minDist
	 *
	 * @local float[x,y,z]	$(c|z)
	 * @local float			$(dr|r)
	 * @local int			$n
	 *
	 * @return float
	 */
	protected function DE($z0, &$minDist)
	{
		$c = $this->julia ? $this->julia_c : $z0; // Julia set has fixed c, Mandelbrot c changes with location
		$z = $z0;

		$dr = 1.0;
		$r = length($z);
		if ($r < $minDist) $minDist = $r;

		for ($n = 0; $n < $this->maxIterations; $n++) {
			$this->powN($z, $r, $dr);
			$z += $c;

			if ($this->radiolaria && $z->y > $this->radiolariaFactor) $z->y = $this->radiolariaFactor;

			$r = length($z);
			if ($r < $minDist) $minDist = $r;
			if ($r > $this->bailout) break;
		}
		return 0.5 * log($r) * $r / $dr;
	}
	

	/**
	 * Intersect bounding sphere
	 *
	 * If we intersect then set the tmin and tmax values to set the start and
	 * end distances the ray should traverse.
	 *
	 * @engine qb
	 *
	 * @param float[3]		$origin
	 * @param float[3]		$direction
	 * @param float			$tmin
	 * @param float			$tmax
	 *
	 * @local bool			$hit
	 * @local float			$(b|c|disc)
	 * @local float			$(sdisc|t0|t1)
	 * @local float			$(minDist)
	 * @local float[3]		$z
	 *
	 * @return bool
	 */
	protected function intersectBoundingSphere($origin, $direction, &$tmin, &$tmax)
	{
		$b = dot($origin, $direction);
		$c = dot($origin, $origin) - $this->bounding;
		$disc = $b*$b - $c;			// discriminant
		$tmin = $tmax = 0.0;

		if ($disc > 0.0) {
			// Real root of disc, so intersection
			$sdisc = sqrt($disc);
			$t0 = -$b - $sdisc;			// closest intersection distance
			$t1 = -$b + $sdisc;			// furthest intersection distance

			if ($t0 >= 0.0) {
				// Ray intersects front of sphere
				$z = $origin + $t0 * $direction;
				$tmin = $this->DE($z, $minDist);
				$tmax = $t0 + $t1;
			} else if ($t0 < 0.0) {
				// Ray starts inside sphere
				$z = $origin;
				$tmin = $this->DE($z, $minDist);
				$tmax = $t1;
			}
			$hit = true;
		}

		return $hit;
	}


	/**
	 * Calculate the gradient in each dimension from the intersection point
	 *
	 * @engine qb
	 * 
	 * @param float[3]		$z
	 * @param float			$e
	 *
	 * @local float			$minDst
	 * @local float[3]		$(z1|z2|z3|z4|z5|z6)
	 * @local float[x,y,z]	$d
	 *
	 * @return float[3]
	 */
	protected function estimate_normal($z, $e) 
	{
		$z1 = $z + array($e, 0, 0);
		$z2 = $z - array($e, 0, 0);
		$z3 = $z + array(0, $e, 0);
		$z4 = $z - array(0, $e, 0);
		$z5 = $z + array(0, 0, $e);
		$z6 = $z - array(0, 0, $e);

		$d->x = $this->DE($z1, $minDst) - $this->DE($z2, $minDst);
		$d->y = $this->DE($z3, $minDst) - $this->DE($z4, $minDst);
		$d->z = $this->DE($z5, $minDst) - $this->DE($z6, $minDst);

		return normalize($d / (2.0*$e));
	}


	/**
	 * Computes the direct illumination for point pt with normal N due to
	 * a point light at light and a viewer at eye.
	 *
	 * @engine qb
	 *
	 * @param float[3]		$pt
	 * @param float[3]		$N
	 * @param float			$specular
	 *
	 * @local float[3]		$diffuse
	 * @local float[3]		$L
	 * @local float[3]		$E
	 * @local float[3]		$R
	 * @local float			$RdE
	 * @local float			$NdotL
	 * @local float			$specular
	 *
	 * @return float32[3]
	 */
	protected function Phong($pt, $N, &$specular)
	{
        $specular = 0.0;
		$L = normalize(vm_mult($this->light, $this->objRotation) - $pt); // find the vector to the light
		$NdotL = dot($N, $L);			// find the cosine of the angle between light and normal

		if ($NdotL > 0.0) {
			// Diffuse shading
			$diffuse = $this->colorDiffuse + abs($N) * $this->colorSpread;
			$diffuse *= $this->colorLight * $NdotL;

			// Phong highlight
			$E = normalize($this->eye - $pt);		// find the vector to the eye
			$R = $L - 2.0 * $NdotL * $N;		// find the reflected vector
			$RdE = dot($R,$E);

			if ($RdE <= 0.0) {
				$specular = $this->specularity * pow(abs($RdE), $this->specularExponent);
            }
		} else {
			$diffuse = $this->colorDiffuse * abs($NdotL) * $this->rimLight;
		}

		return ($this->colorAmbient * $this->colorAmbientIntensity) + $diffuse;
	}


	/**
	 * Define the ray direction from the pixel coordinates
	 *
	 * @engine qb
	 *
	 * @param float32[x,y]		$p
	 *
	 * @local float32[3]		$direction
	 *
	 * @return float32[3]
	 */
	protected function rayDirection($p) 
	{
		$direction[0] = 2.0 * $this->aspectRatio * $p->x / $this->width - $this->aspectRatio;
		$direction[1] = -2.0 * $p->y / $this->height + 1.0;
		$direction[2] = -2.0 * $this->cameraZoomExponential;
		return normalize(vm_mult(vm_mult($direction, $this->viewRotation), $this->objRotation));
	}


	/**
	 * Calculate the output colour for each input pixel
	 *
	 * @engine qb
	 *
	 * @param float[x,y]		$pixel
	 *
	 * @local float				$(tmin|tmax)
	 * @local float[3]			$(rayDirection|lightDirection)
	 * @local float[r,g,b,a]	$color
	 * @local float[3]			$(ray|normal)
	 * @local float				$(dist|ao|minDist|minDist2|rayLength|eps|f)
	 * @local float				$specular
	 * @local int				$(i|j|maxSteps)
	 *
	 * @return float[4]
	 */
	protected function renderPixel($pixel)
	{
		$rayDirection = $this->rayDirection($pixel);
        $color->rgb = $this->colorBackground;
        $color->a = $this->colorBackgroundTransparency;

		if ($this->intersectBoundingSphere($this->eye, $rayDirection, $tmin, $tmax)) {
			$ray = $this->eye + $tmin * $rayDirection;

			$minDist = 4.0;
			$rayLength = $tmin;
			$eps = self::MIN_EPSILON;

			// number of raymarching steps scales inversely with factor
			$maxSteps = $this->stepLimit / $this->epsilonScale;

			for ($i = 0; $i < $maxSteps; ++$i) {
				$dist = $this->DE($ray, $minDist);

				// March ray forward
				$f = $this->epsilonScale * $dist;
				$ray += $f * $rayDirection;
				$rayLength += $f * $dist;

				// Are we within the intersection threshold or completely missed the fractal
				if ($dist < $eps || $rayLength > $tmax) break;

				// Set the intersection threshold as a function of the ray length from the camera
				$eps = max(self::MIN_EPSILON, $this->pixelScale * $rayLength);
			}

			// Found intersection?
			if ($dist < $eps) {
				$ao	= 1.0 - clamp(1.0 - $minDist * $minDist, 0.0, 1.0) * $this->ambientOcclusion;

				if ($this->phong) {
					$normal = $this->estimate_normal($ray, $eps/2.0);
					$specular = 0.0;
					$color->rgb = $this->Phong($ray, $normal, $specular);

					if ($this->shadows > 0.0) {
						// The shadow ray will start at the intersection point and go
						// towards the point light. We initially move the ray origin
						// a little bit along this direction so that we don't mistakenly
						// find an intersection with the same point again.
						$lightDirection = normalize(($this->light - $ray) * $this->objRotation);
						$ray += $normal * $eps * 2.0;

						$dist = 4.0;

						for ($j = 0; $j < $maxSteps; ++$j) {
							$dist = $this->DE($ray, $minDist2);

							// March ray forward
							$f = $this->epsilonScale * $dist;
							$ray += $f * $lightDirection;

							// Are we within the intersection threshold or completely missed the fractal
							if ($dist < $eps || dot($ray, $ray) > $this->bounding * $this->bounding) break;
						}

						// Again, if our estimate of the distance to the set is small, we say
						// that there was a hit and so the source point must be in shadow.
						if ($dist < $eps) {
							$color->rgb *= 1.0 - $this->shadows;
						} else {
							// Only add specular component when there is no shadow
							$color->rgb += $specular;
						}
					} else {
						$color->rgb += $specular;
					}
				} else {
					// Just use the base colour
					$color->rgb = $this->colorDiffuse;
				}

				$ao *= 1.0 - ($i / $maxSteps) * $this->ambientOcclusionEmphasis * 2.0;
				$color->rgb *= $ao;
                $color->a = 1.0;
			}
		}

		return $color;
	}


	// Common values used by all pixels
	/**
	 * The main loop
	 *
	 * @engine qb
	 *
	 * @param image			$dst
	 *
	 * @local int			$width
	 * @local int			$height
	 * @local int			$x
	 * @local int			$y
	 * @local float			$i
	 * @local float			$j
	 * @local float[4]		$c
	 * @local float			$(c1|c2|c3)
	 * @local float			$(s1|s2|s3)
	 * @local float[3][3]	$(viewRotationX|viewRotationY|viewRotationZ)
	 * @local float[3][3]	$(objRotationX|objRotationY|objRotationZ)
	 * @local float[x,y]	$coord
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
		$c1 = cos(deg2rad(-$this->cameraRotation->x));
		$s1 = sin(deg2rad(-$this->cameraRotation->x));
		$viewRotationY = array($c1, 0, $s1, 0, 1, 0, -$s1, 0, $c1);

		$c2 = cos(deg2rad(-$this->cameraRotation->y));
		$s2 = sin(deg2rad(-$this->cameraRotation->y));
		$viewRotationZ = array($c2, -$s2, 0, $s2, $c2, 0, 0, 0, 1);

		$c3 = cos(deg2rad(-$this->cameraRotation->z));
		$s3 = sin(deg2rad(-$this->cameraRotation->z));
		$viewRotationX = array(1, 0, 0, 0, $c3, -$s3, 0, $s3, $c3);

		$this->viewRotation = mm_mult($viewRotationZ, mm_mult($viewRotationY, $viewRotationX));

		// Object rotation
		$c1 = cos(deg2rad(-$this->rotation->x));
		$s1 = sin(deg2rad(-$this->rotation->x));
		$objRotationY = array($c1, 0, $s1, 0, 1, 0, -$s1, 0, $c1);

		$c2 = cos(deg2rad(-$this->rotation->y));
		$s2 = sin(deg2rad(-$this->rotation->y));
		$objRotationZ = array($c2, -$s2, 0, $s2, $c2, 0, 0, 0, 1);

		$c3 = cos(deg2rad(-$this->rotation->z));
		$s3 = sin(deg2rad(-$this->rotation->z));
		$objRotationX = array(1, 0, 0, 0, $c3, -$s3, 0, $s3, $c3);

		$this->objRotation = mm_mult($objRotationZ, mm_mult($objRotationY, $objRotationX));
		
		$this->eye = ($this->cameraPosition + $this->cameraPositionFine);
		if ($this->eye == array(0, 0, 0)) $this->eye = array(0, 0.0001, 0);

		$this->eye = vm_mult($this->eye, $this->objRotation);
		
		$this->cameraZoomExponential = exp($this->cameraZoom);

		// Super sampling
		$this->sampleStep = 1.0 / $this->antialiasing;
		$this->sampleContribution = 1.0 / pow($this->antialiasing, 2.0);
		$this->pixelScale = 1.0 / max($this->width, $this->height);
		
		$y = fork($height);
		$coord->y = $y + 0.5;
		
		for ($x = 0, $coord->x = 0.5; $x < $width; $x++, $coord->x++) {
			$c = array(0, 0, 0, 0);
			if ($this->antialiasing > 1) {
				// Average antialiasing^2 points per pixel
				for ($i = 0.0; $i < 1.0; $i += $this->sampleStep)
					for ($j = 0.0; $j < 1.0; $j += $this->sampleStep)
						$c += $this->sampleContribution * $this->renderPixel(array($coord->x + $i, $coord->y + $j));
			} else {
				$c = $this->renderPixel($coord);
			}
			// Return the final color which is still the background color if we didn't hit anything.
			$dst[$y][$x] = $c;
		}
	}

}

ini_set("qb.allow_debugger_inspection", 0);
ini_set("qb.column_major_matrix", 1);

$folder = dirname(__FILE__);
$output = imagecreatetruecolor(480, 480);
$correct_path = "$folder/output/mandelbulb.correct.png";
$incorrect_path = "$folder/output/mandelbulb.fork.incorrect.png";

$rayTracer = new Mandelbulb;
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
