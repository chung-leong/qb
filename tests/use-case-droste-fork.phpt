--TEST--
Use case: Droste Effect (multicore)
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

// Ported from Pixel Bender
// Droste by Tom Beddard 
// Url: http://www.adobe.com/cfusion/exchange/index.cfm?event=extensionDetail&extid=1700080

// Based on the Mathmap version described in the Escher Droste Flickr group:
// http://www.flickr.com/groups/escherdroste/
//
// Original Droste formula by: 
// Ben Reichardt: http://www.flickr.com/photos/breic
// Additional Features and Mathmap 1.2 conversion by:
// Josh Sommers: http://www.flickr.com/photos/joshsommers
// 
// The math behind the transformation:
// http://escherdroste.math.leidenuniv.nl/
// http://www.josleys.com/articles/printgallery.htm
// 
// Examples on using the parameters and further updates to the code at:
// http://www.subblue.com/projects/droste
//
// Changelog:
// 17/11/2008 v1.0: Initial release
// 25/09/2009 v1.1: Tidied up the code and controls a little.
//				    Added a rotateSpin parameter to rotate the untransformed image. Works well with rotatePolar
//                  Added anti-aliasing option. 1 = no AA, 2 = 4x samples, 3 = 9x samples.
//
// Quickstart
// 
// These are the steps you should follow for best results:
// 1) Set the size to match the source image
// 2) Adjust the shift so that the spiral is centered on the area you want to repeat
// 3) If the source image is a 24-bit PNG with a transparent centre section 
//    (like a picture frame) then set transparentInside to 1.
//    If the image has a transparent outside (like a flower cut-out) set enableOutsideTransparency to 1
// 4) Adjust the inner radius in combination with tweaking the shift values until the repeated section
//    aligns nicely.
// 5) Adjust the center parameter to frame the overall image.
//
// Tips
// 
// Periodicity:
//   This sets the number of times the image is repeat on each level of the spiral. For 
//   picture frame type images leave this as 1. For circular images small adjustments 
//   might be needed to get a nice repeating spiral but it is always worth enabling
//   the periodicityAuto setting.
//
// Levels:
//   Increase the levelStart so that the image fills the outer area.
//   Increase the levels until the centre of the spiral has filled in.
//
// Strands:
//   This sets the number of arms on the spiral. When set greater than 1 and applied to an image 
//   like a flower that has an outside transparency then strandMirror for seamless tiling.
//

class Droste {

	/** @var float 				The inner radius of the repeating annular */
	public $radiusInside = 25.0;
    
	/** @var float 				The outer radius of the repeating annular */
	public $radiusOutside = 100.0;

	/** @var float 				The number of image the image is repeated on each level */
    public $periodicity = 1.0;
    
    /** @var int				The number of strands of the spiral */
    public $strands = 1;
    
    /** @var bool				Smoother repeating when using more than one strand */
	public $strandMirror = true;

	/** @var float				Overall image magnification */
	public $zoom = 0.0;

	/** @var float				Overall image rotation */
	public $rotate = 0.0;

    /** @var float[x,y]			Panning of the image in the output frame */
    public $center = array(0.0, 0.0);
    
    /** @var float[x,y]			Shift the centre of the spiral */
    public $centerShift = array(0.0, 0.0);
    
	/** @var float[r,g,b,a]		Set the RGBA background colour */
	public $backgroundRGBA = array(0, 0, 0, 1);

	/** @var int				The number of repeating levels of the spiral */
	public $levels = 9;

	/** @var int			    The starting spiral level */
	public $levelStart = 3;

	/** @var bool    			Enable for images with transparent middle areas (such as a picture frame) */
	public $transparentInside = true;

	/** @var bool				Enable for images with transparent areas around the outside */
	public $transparentOutside = true;

	/** @var bool				Enable to unroll the circular annular of the image */
	public $twist = true;

	/** @var bool				Automatically set the ideal periodicity for the current radius settings */
    public $periodicityAuto = false;

	/** @var float				Polar rotation */    
	public $rotatePolar = 0.0;

	/** @var float				Spin mapped image. Best used with polar rotation */
	public $rotateSpin = 0.0;

    
    /**
     * @engine qb
     *
     * @param image				$output
     * @param image				$image 
     *
     * @local uint32			$(x|y)
     * @local int				$(iteration|maxIteration)
     * @local bool				$tileBasedOnTransparency
     * @local float[r,g,b,a]	$(color|colorSoFar)
	 * @local float[2][2]		$imageSpin
     * @local float				$(height|width)
     * @local float				$(sign|signTransparentOutside|alphaRemaining|theta|radius)
     * @local float				$(r1|r2|p1|p2|sc|ss)
     * @local float[min,max]	$[xy]Bounds
     * @local float[x,y]		$.*				All other local variables are coordinate of some sort
     *
     */
    
	public function generate(&$output, $image) {
		$colorSoFar = 0;
		// Obtain dimension of output image
		$height = count($output);
		$width = count($output[0]);
	
		$r1 = $this->radiusInside * 0.01;
		$r2 = $this->radiusOutside * 0.01;
		$p1 = max(0.001, $this->periodicity);	// Prevent divide by zero
		$p2 = max($this->strands, 0.0001);
		$tileBasedOnTransparency = ($this->transparentInside || !$this->transparentOutside);
		$signTransparentOutside = ($this->transparentOutside) ? 1.0 : -1.0;
		
		$shift = 1.0 + $this->centerShift * 0.01;
		$center->x = $width / 2 + $this->center->x * ($width / 2) / 100;
		$center->y = $height / 2 + $this->center->y * ($height / 2) / 100;
				
		$minDimension = min($width, $height) / 2;
				
		$I = array(0.0, 1.0);
		
		// Autoset periodicity
		if($this->periodicityAuto) {
			$p1 = $p2 * 0.5 * (1.0 + sqrt(1.0 - pow(log($r2/$r1)/M_PI, 2.0)));
		}
		
		// Set rotation
		$rotate->x = ($p1 > 0.0) ? -deg2rad($this->rotate) : deg2rad($this->rotate);
		$rotate->y = 0;
		
		$sc = cos(deg2rad($this->rotateSpin));
		$ss = sin(deg2rad($this->rotateSpin));
		$imageSpin = array($sc, $ss, -$ss, $sc);
		
		// Set zoom
		$zoom->x = (exp($this->zoom) + $this->radiusInside - 1.0) * 0.01;
		$zoom->y = 0.0;
		
		// Scale viewport pixels to complex plane
		if($this->twist) {
			$xBounds = array(-$r2, $r2);
			$yBounds = array(-$r2, $r2);
		} else {
			$xBounds = array(-log($r2/$r1), log($r2/$r1));
			$yBounds = array(0.0, 2.1 * M_PI);
		}
		
		$xyMiddle->x = 0.5 * ($xBounds->max + $xBounds->min);
		$xyMiddle->y = 0.5 * ($yBounds->max + $yBounds->min);
		$xyRange->x = $xBounds->max - $xBounds->min;
		$xyRange->y = $yBounds->max - $yBounds->min;
		$xyRange->x = $xyRange->y * ($width / $height);
		$xBounds->min = $xyMiddle->x - 0.5 * $xyRange->x;
		$xBounds->max = $xyMiddle->x + 0.5 * $xyRange->x;
				
		$y = fork($height);
		
		for($x = 0; $x < $width; $x++) {
			$sign = 0;
			$alphaRemaining = 1.0;
			$colorSoFar = 0;
			
			$z->x = $xBounds->min + ($xBounds->max - $xBounds->min) * ($x - $center->x + $width / 2.0) / $width;
			$z->y = $yBounds->min + ($yBounds->max - $yBounds->min) * ($y - $center->y + $height / 2.0) / $height;
			
			// Only allow for procedural zooming/scaling in the standard coordinates
			if($this->twist) {
				$z = $xyMiddle + cmult(cdiv(($z - $xyMiddle), $zoom), cexp(cmult(-$I, $rotate)));
			}
			
			if($this->rotatePolar != 0.0) {
				$theta = deg2rad($this->rotatePolar);
			
				$div->x = (1.0 + pow($z->x, 2.0) + pow($z->y, 2.0) + ((1.0 - pow($z->x, 2.0) - pow($z->y, 2.0)) * cos($theta)) - (2.0 * $z->x * sin($theta))) / 2.0;
				$z->x = $z->x * cos($theta) + ((1.0 - pow($z->x, 2.0) - pow($z->y, 2.0)) * sin($theta) / 2.0);
				$z = cdiv($z, $div);
			}
			
			$z = vm_mult($z, $imageSpin);
			
			if ($this->twist) {
				$z = clog(cdiv($z, array($r1, 0.0)));
			}
			
			// Start Droste-effect code
			$alpha->x = atan(($p2/$p1) * (log($r2/$r1) / (2 * M_PI)));
			$alpha->y = 0.0;
			$f->x = cos($alpha->x);
			$f->y = 0.0;
			$beta = cmult($f, cexp(cmult($alpha, $I)));
			
			// The angle of rotation between adjacent annular levels
			$angle->x = -2 * M_PI * $p1;
			$angle->y = 0.0;
			
			if($p2 > 0.0) {
				$angle = -$angle;
			}
			if($this->strandMirror) {
				$angle /= $p2;
			}
			
			$z = cdiv(cmult(array($p1, 0.0), $z), $beta);
			$z = cmult(array($r1, 0.0), cexp($z));
			// End Droste-effect code
			
			// Start drawing
			if($tileBasedOnTransparency && $this->levelStart > 0) {
				if($this->transparentOutside) {
					$ratio = cmult(array($r2/$r1, 0.0), cexp(cmult($angle,  $I)));
				} else {
					$ratio = cmult(array($r1/$r2, 0.0), cexp(cmult($angle, -$I)));
				}
				$z = cmult($z, cpow($ratio, array($this->levelStart, 0)));
			}
			
			$iteration = 0;
			
    		$d = $minDimension * ($z + $shift);
        	$sign = 0;
	        if($tileBasedOnTransparency || $iteration == 0) {
				$color = sample_bilinear($image, $d->x, $d->y);
	            $colorSoFar += $color * ($color->a * $alphaRemaining);
	            $alphaRemaining *= (1.0 - $colorSoFar->a);
	        }
    
    		if($tileBasedOnTransparency) {
    			if($alphaRemaining > 0.0) {
    				$sign = $signTransparentOutside;
    			}
    		} else {
        		if($iteration > 0) {
        			$colorSoFar = sample_bilinear($image, $d->x, $d->y);
        		}
        		$radius = length($z);
        		$sign = sign($radius - $r1);
			}
			$iteration++;
			
			if($sign < 0) {
				$ratio = cmult(array($r2/$r1, 0.0), cexp(cmult($angle, $I)));
			} else if($sign > 0) {
				$ratio = cmult(array($r1/$r2, 0.0), cexp(cmult($angle, -$I)));
			}
			
			$iteration = $this->levelStart;
			$maxIteration = $this->levels + $this->levelStart - 1;
			
			while($sign != 0 && $iteration < $maxIteration) {
				$z = cmult($z, $ratio);
				
        		$d = $minDimension * ($z + $shift);
	        	$sign = 0;
		        if($tileBasedOnTransparency || $iteration == 0) {
					$color = sample_bilinear($image, $d->x, $d->y);
		            $colorSoFar += $color * ($color->a * $alphaRemaining);
		            $alphaRemaining *= (1.0 - $colorSoFar->a);
		        }
        
        		if($tileBasedOnTransparency) {
        			if($alphaRemaining > 0.0) {
        				$sign = $signTransparentOutside;
        			}
        		} else {
            		if($iteration > 0) {
            			$colorSoFar = sample_bilinear($image, $d->x, $d->y);
            		}
            		$radius = length($z);
            		$sign = sign($radius - $r1);
				}
				$iteration++;
			}
			if($colorSoFar->a < 1.0) {
				$colorSoFar = mix($colorSoFar, $this->backgroundRGBA, 1.0 - $colorSoFar->a);
			}
			$output[$y][$x] = $colorSoFar;
		}
	}
	    
}

// so the script wouldn't slow to a grind if xdebug is active
ini_set("qb.allow_debugger_inspection", 0);

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/ipad.png");
$output = imagecreatetruecolor(680, 680);
$correct_path = "$folder/output/droste.correct.png";
$incorrect_path = "$folder/output/droste.fork.incorrect.png";

$droste = new Droste;
$droste->twist = true;
$droste->periodicity = 1;
$droste->transparentOutside = true;
$droste->transparentInside = true;
$droste->radiusInside = 40;
$droste->radiusOutside = 100;
$droste->zoom = 3;
$droste->rotate = 30;
$droste->center = array(12, 8);
$droste->centerShift = array(-20, 24);
$droste->generate($output, $image);

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
		//$match = false;
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
