--TEST--
Use case: Ray Tracer
--SKIPIF--
<?php 
	if(function_exists('imagepng') < 0) print 'skip PNG function not available';
?>
--FILE--
<?php

// Ported from Pixel Bender
// Raytracer by mrwelsh 
// Url: http://www.adobe.com/cfusion/exchange/index.cfm?event=extensionDetail&loc=en_us&extid=1634018

define('NUM_SPHERES', 35);

class RayTracer {

	/** @var float32 */
	public $viewPlaneDistance = 2.0;
	
	/** @var float32[3] */
	public $lightPos = array(0.0, 2.0, -4.0);
	
	/** @var float32[3] */
	public $sphere0Position = array(0.0, 2.0, -10.0);
	
	/** @var float32 */
	public $sphere0Radius = 2.0;
	
	/** @var float32[3] */
	public $sphere0Color = array(0.8, 0.8, 0.8);
	
	/** @var float32[4] */
	public $sphere0Material = array(0.05, 0.1, 1.0, 1.0);
	
	const SPECULAR_EXPONENT			= 50.0;
	const MAX_RAY_SHOTS				= 4;
    
    /** @var float32[NUM_SPHERES][x, y, z] */
	public $spherePositions;
    
    /** @var float32[NUM_SPHERES] */
	public $sphereRadii;
    
    /** @var float32[NUM_SPHERES][r, g, b] */
	public $sphereColors;
    
    /** @var float32[NUM_SPHERES][ambient, diffuse, specular, reflectivity] */
	public $sphereMaterials;
    
	/** 
	* @engine qb 
	* 
	* @local uint32			$i
	* @local float32		$num
	*/
	private function initialize() {
	    // initialize our sphere parameters
        $this->spherePositions[0] = $this->sphere0Position;
        $this->sphereRadii[0] = $this->sphere0Radius;
        $this->sphereColors[0] = $this->sphere0Color;
        $this->sphereMaterials[0] = $this->sphere0Material;

		$this->spherePositions[1] = array(0.0, -1003.0, -8.0);
		$this->sphereRadii[1] = 1000.0;
		$this->sphereColors[1] = array(0.6, 0.6, 0.6);
		$this->sphereMaterials[1] = array(0.1, 0.8, 0.5, 0.5);
		
		// let's make a bunch of fakely random spheres
		for($i = 2; $i < NUM_SPHERES; $i ++) {
			$num = $i * 11;
			$this->spherePositions[$i]->x = sin($num / 5.0) * 6.0;
			$this->spherePositions[$i]->y = sin($num / 4.1) * 2.5;
			$this->spherePositions[$i]->z = -18.0 - sin($num / 3.1 + 1.2) * 10.0;
			$this->sphereRadii[$i] = pow(sin($num / 1.34 + 65.3) * 0.5 + 0.5, 3.0) * 1.0 + 0.2;
			$this->sphereColors[$i]->r = cos($num / 2.1 + 1.3) * 0.5 + 0.5;
			$this->sphereColors[$i]->g = cos($num / 0.1 + 1.3) * 0.5 + 0.5;
			$this->sphereColors[$i]->b = cos($num / 5.1 + 6.3) * 0.5 + 0.5;
			$this->sphereMaterials[$i]->ambient = 0.1;
			$this->sphereMaterials[$i]->diffuse = 0.7;
			$this->sphereMaterials[$i]->specular = 1.0;
			$this->sphereMaterials[$i]->reflectivity = pow(sin($num / 2.1 + 1.243) * 0.5 + 0.5, 5.0);
		}
	}

	/** 
	 * generate():	generate raytraced image
	 *
	 * @engine qb
	 *
	 * @param image3			$image
	 *
	 * @local uint32			$width
	 * @local uint32			$height
	 * @local uint32			$x
	 * @local uint32			$y	 
	 * @local float32[3]		$dst
	 * @local float32[x, y, z]	$origin
	 * @local float32[x, y, z]	$dir
     * @local float32[3]		$sphereHit		hit point relative to sphere
     * @local float32[3]		$n				surface normal
     * @local float32[3]		$lightVector	surface to light
     * @local float32			$lightVectorLen
     * @local float32[3]		$l				normalized light vector
	 * @local float32[3]		$lReflect		reflected off surface
	 * @local float32[3]		$dirReflect
	 * @local float32[3]		$colorScale
	 * @local float32[3]		$sphereColor
	 * @local float32[ambient, diffuse, specular, reflectivity]	$sphereMaterial
	 * @local float32			$specular
	 * @local float32			$diffuse
	 * @local float32			$lightVal
	 * @local bool				$hit
	 * @local float32[3]		$hitPoint
	 * @local float32			$t
	 * @local uint32			$sphereNum
	 * @local bool				$shadowTest
	 * @local int32				$rayShots
	 * @local float32			$phi
	 * @local float32			$u
	 * @local float32			$v
	 * @local float32 			$curT
	 * @local float32			$B
	 * @local float32			$C
	 * @local float32			$disc
	 * @local float32[3]		$sphereToOrigin
	 * @local uint32			$i
	 */
	public function generate(&$image) {
		$this->initialize();
		
		// obtain dimension of output image
		$height = count($image);
		$width = count($image[0]);
		
		for($y = 0; $y < $height; $y++) {
			for($x = 0; $x < $width; $x++) {
				$dst = 0;
				$origin = 0;
				
		        // calculate direction vector for this pixel        
		        $dir->x = 2.0 * $x / $width - 1.0;
		        $dir->y = -2.0 * $y / $height + 1.0;
				$dir->z = -$this->viewPlaneDistance;
        
		        $colorScale = 1;
        		$rayShots = self::MAX_RAY_SHOTS;
        
				while($rayShots > 0 ) {
		            // let's make sure dir is properly normalized
					$dir = normalize($dir);
		            
					// INTERSECTION TEST
					// find the first sphere we intersect with
					$hit = false;
					$t = 99999.0;
			        
			        // cycle through all spheres and find the smallest t>0 that we hit
					for($i = 0; $i < NUM_SPHERES; $i++) {
						$sphereToOrigin = $origin - $this->spherePositions[$i];
						$B = dot($sphereToOrigin, $dir);
						$C = dot($sphereToOrigin, $sphereToOrigin) - $this->sphereRadii[$i] * $this->sphereRadii[$i];
					
						$disc = $B * $B - $C;
						if($disc > 0.0) {
							$curT = -$B - sqrt($disc);
							if($curT > 0.0 && $curT < $t) {
								$sphereNum = $i;
								$t = $curT;
								$hit = true;
							}
						}
					}
					
					if($hit) {
						$hitPoint = $origin + $dir * $t;
		                $sphereColor = $this->sphereColors[$sphereNum];
		                $sphereMaterial = $this->sphereMaterials[$sphereNum];
						$sphereHit = $hitPoint - $this->spherePositions[$sphereNum];
						$n = $sphereHit / $this->sphereRadii[$sphereNum];				// normal at the point we hit
		                $lightVector = $this->lightPos - $hitPoint;						// hit point to light
						$lightVectorLen = length($lightVector);
		                $l = $lightVector / $lightVectorLen;
		                
						// SHADOW TEST
						// fire a ray from our hit position towards the light
						$shadowTest = false;
						$t = 99999.0;
						for($i = 0; $i < NUM_SPHERES; $i++) {
							$sphereToOrigin = $hitPoint - $this->spherePositions[$i];
							$B = dot($sphereToOrigin, $l);
							$C = dot($sphereToOrigin, $sphereToOrigin) - $this->sphereRadii[$i] * $this->sphereRadii[$i];
						
							$disc = $B * $B - $C;
							if($disc > 0.0) {
								$curT = -$B - sqrt($disc);
								if($curT > 0.0 && $curT < $t) {
									$t = $curT;
									$shadowTest = true;
								}
							}
						}
		                
						if(!$shadowTest) {					// if we didn't hit anything, we can see the light
							$shadowTest = 1;
						} else if($t < $lightVectorLen)	{	// if we hit something before the light, we are in shadow
							$shadowTest = 0;
						}
		                
		                $diffuse = dot($l, $n);
		
						$lReflect = reflect($l, $n);		// reflect the light vector
						$specular = dot($dir, $lReflect);
						
		                $diffuse = max($diffuse, 0.0);
						$specular = pow(max($specular, 0.0), self::SPECULAR_EXPONENT);
						
		                // ground checkboard texture
						if($sphereNum == 1) {
							$phi = acos(-$n[0]);
							$u = acos($n[2] / sin($phi)) / (2.0 * M_PI);
							$v = $phi / M_PI;
		                 
							// we could do sample_linear here to do some actual texturing. :)
							$sphereColor *= ((floor($u * 2000.0) + floor($v * 2000.0)) % 2.0 == 0.0) ? 0.5 : 1.0;
		                }
		                
						// finally, blend our color into this pixel
						$lightVal = $sphereMaterial->ambient + float32($shadowTest) * (($diffuse * $sphereMaterial->diffuse) + ($specular * $sphereMaterial->specular));
						$dst += $colorScale * $lightVal * $sphereColor;
		                
		                // reflection
						if($sphereMaterial->reflectivity > 0.0) {
							$dirReflect = reflect($dir, $n);		// reflect our view vector
							$dirReflect = normalize($dirReflect);
							
		                    // originate at our hit position, fire at reflected angle
							$origin = $hitPoint;
							$dir = $dirReflect;
							$rayShots--;
		                    
		                    // blend according to reflectivity
							$colorScale *= $sphereMaterial->reflectivity * $sphereColor;
		                } else {
							$rayShots = 0;
						}
					} else {
						$rayShots = 0;
					}
		        }
		        $image[$y][$x] = $dst;
		    }
		}
	}
}

// so the script wouldn't slow to a grind if xdebug is active
ini_set("qb.allow_debugger_inspection", 0);

qb_compile();

$folder = dirname(__FILE__);
$output = imagecreatetruecolor(512, 512);
$correct_path = "$folder/output/raytrace.correct.png";
$incorrect_path = "$folder/output/raytrace.incorrect.png";

$rayTracer = new RayTracer;
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
function image_diff($img1, $img2) {
	$img2 -= $img1;
	return array_sum($img2);
}

if(file_exists($correct_path)) {
	$correct_md5 = md5_file($correct_path);
	$output_md5 = md5($output_png);
	if($correct_md5 == $output_md5) {
		// exact match
		$match = true;
	} else {
		$correct_output = imagecreatefrompng($correct_path);
		$diff = image_diff($output, $correct_output);
		if($diff < 0.05) {
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
		echo "INCORRECT\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
}


?>
--EXPECT--
CORRECT