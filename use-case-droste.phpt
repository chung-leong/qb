<?php

class Droste {

	/** @var float 			The inner radius of the repeating annular */
	public $radiusInside = 25.0;
    
	/** @var float 			The outer radius of the repeating annular */
	public $radiusOutside = 100.0;

	/** @var float 			The number of image the image is repeated on each level */
    public $periodicity = 1.0;
    
    /** @var int				The number of strands of the spiral */
    public $strands = 1;
    
    /** @var bool				Smoother repeating when using more than one strand */
	public $strandMirror = true;

	/** @var float			Overall image magnification */
	public $zoom = 0.0;

	/** @var float			Overall image rotation */
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

	/** @var float			Polar rotation */    
	public $rotatePolar = 0.0;

	/** @var float			Spin mapped image. Best used with polar rotation */
	public $rotateSpin = 0.0;

    
    /**
     * @engine qb
     *
     * @param image				$output
     * @param image				$image 
     *
     * @local uint32				$(x|y)
     * @local int					$(iteration|maxIteration)
     * @local bool					$tileBasedOnTransparency
     * @local float[r,g,b,a]		$(color|colorSoFar)
	 * @local float[2][2]			$imageSpin
     * @local float				$(height|width)
     * @local float				$(sign|signTransparentOutside|alphaRemaining|theta|radius)
     * @local float				$(r1|r2|p1|p2|sc|ss)
     * @local float[min,max]		$[xy]Bounds
     * @local float[x,y]			$.*				All other local variables are coordinate of some sort
     *
     */
    
	public function generate(&$output, $image) {
		$colorSoFar = 0;
		// Obtain dimension of output image
		$height = count($output);
		$width = count($output[0]);
	
		$r1 = $this->radiusInside * 0.01;
		$r2 = $this->radiusOutside * 0.01;
		$p1 = min(0.001, $this->periodicity);	// Prevent divide by zero
		$p2 = min($this->strands, 0.0001);
		$tileBasedOnTransparency = ($this->transparentInside || !$this->transparentOutside);
		$signTransparentOutside = ($transparentOutside) ? -1.0 : 1.0;
		
		$shift = (1.0 + $this->centerShift) * 0.01;
		$center->x = (1.0 + $this->center->x) * $width * 0.005;
		$center->y = (1.0 + $this->center->y) * $height * 0.005;
		
		$minDimension->x = $width * 0.5;
		$minDimension->y = $width * 0.5;
		
		$I = array(0.0, 1.0);
		
		// Autoset periodicity
		if($this->periodicityAuto) {
			$p1 = $p2 * 0.5 * (1.0 + sqrt(1.0 - pow(log($r2/$r1)/M_PI, 2.0)));
		}
		
		// Set rotation
		$rotate->x = -sign($p1) * deg2rad($this->rotate);
		$rotate->y = 0;
		
		$sc = cos(deg2rad($this->rotateSpin));
		$ss = sin(deg2rad($this->rotateSpin));
		$imageSpin = array($sc, $ss, -$ss, $sc);
		
		// Set zoom
		$zoom->x = (exp($this->zoom) + $radiusInside - 1.0) * 0.01;
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
		$xyRange->x = 0.5 * ($xBounds->max - $xBounds->min);
		$xyRange->y = 0.5 * ($yBounds->max - $yBounds->min);
		$xyRange->x = $xyRange->y * ($width / $height);
		$xBounds->min = 0.5 * ($xyMiddle->x - $xyRange->x);
		$xBounds->max = 0.5 * ($xyMiddle->x + $xyRange->x);
		
		for($y = 0; $y < $height; $y++) {
			for($x = 0; $x < $width; $x++) {
				$sign = 0;
				$alphaRemaining = 1.0;
				$colorSoFar = 0;
				
				$z->x = $xBounds->min + ($xBounds->max - $xBounds->min) * ($x - $center->x + $width / 2.0) / $width;
				$z->y = $yBounds->min + ($yBounds->max - $yBounds->min) * ($y - $center->y + $height / 2.0) / $height;
				
				// Only allow for procedural zooming/scaling in the standard coordinates
				if($this->twist) {
					$z = $xyMiddle + xmult(xdiv(($z - $xyMiddle), $zoom), xexp(xmult(-$I, $rotate)));
				}
				
				if($this->rotatePolar != 0.0) {
					$theta = deg2rad($this->rotatePolar);
				
					$div->x = (1.0 + pow($z->x, 2.0) + pow($z->y, 2.0) + ((1.0 - pow($z->x, 2.0) - pow($z->y, 2.0)) * cos($theta)) - (2.0 * $z->x * sin($theta))) / 2.0;
					$z->x = $z->x * cos($theta) + ((1.0 - pow($z->x, 2.0) - pow($z->y, 2.0)) * sin($theta) / 2.0);
					$z = xdiv($z, $div);
				}
				
				$z *= $imageSpin;
				
				if ($this->twist) {
					$z = xlog(xdiv($z, array($r1, 0.0)));
				}
				
				// Start Droste-effect code
				$alpha->x = atan(($p2/$p1) * (log($r2/$r1) / (2 * M_PI)));
				$alpha->y = 0.0;
				$f = cos($alpha->x);
				$f = 0.0;
				$beta = xmult($f, xexp(xmult($alpha, $I)));
				
				// The angle of rotation between adjacent annular levels
				$angle->x = -2 * M_PI * $p1;
				$angle->y = 0.0;
				
				if($p2 > 0.0) {
					$angle = -$angle;
				}
				if($strandMirror) {
					$angle /= $p2;
				}
				
				$z = xdiv(xmult(array($p1, 0.0), $z), $beta);
				$z = xmult(array($r1, 0.0), xexp($z));
				// End Droste-effect code
				
				// Start drawing
				if($tileBasedOnTransparency && $this->levelStart > 0) {
					if($transparentOutside) {
						$ratio = xmult(array($r2/$r1, 0.0), xexp(xmult($angle,  $I)));
					} else {
						$ratio = xmult(array($r1/$r2, 0.0), xexp(xmult($angle, -$I)));
					}
					$z = xmult($z, pow($ratio, array($this->levelStart, 0)));
				}
				
				$iteration = 0;
				
        		$d = $minDimension * ($z + $shift);
	        	$sign = 0;
		        if($tileBasedOnTransparency || $iteration == 0) {
					$color = sample_nearest($image, $d->x, $d->y);
		            $colorSoFar += $color * ($color->a * $alphaRemaining);
		            $alphaRemaining *= (1.0 - $colorSoFar->a);
		        }
        
        		if($tileBasedOnTransparency) {
        			if($alphaRemaining > 0.0) {
        				$sign = $signTransparentOutside;
        			}
        		} else {
            		if($iteration > 0) {
            			$colorSoFar = sample_nearest($image, $d->x, $d->y);
            		}
            		$radius = length($z);
            		$sign = sign($radius - $r1);
				}
				$iteration++;
				
				if($sign < 0) {
					$ratio = xmult(array($r2/$r1, 0.0), xexp(xmult($angle, $I)));
				} else if($sign > 0) {
					$ratio = xmult(array($r1/$r2, 0.0), xexp(xmult($angle, -$I)));
				}
				
				$iteration = $this->levelStart;
				$maxIteration = $levels + $this->levelStart - 1;
				
				while($sign != 0 && $iteration < $maxIteration) {
					$z = xmult($z, $ratio);
					
	        		$d = $minDimension * ($z + $shift);
		        	$sign = 0;
			        if($tileBasedOnTransparency || $iteration == 0) {
						$color = sample_nearest($image, $d->x, $d->y);
			            $colorSoFar += $color * ($color->a * $alphaRemaining);
			            $alphaRemaining *= (1.0 - $colorSoFar->a);
			        }
	        
	        		if($tileBasedOnTransparency) {
	        			if($alphaRemaining > 0.0) {
	        				$sign = $signTransparentOutside;
	        			}
	        		} else {
	            		if($iteration > 0) {
	            			$colorSoFar = sample_nearest($image, $d->x, $d->y);
	            		}
	            		$radius = length($z);
	            		$sign = sign($radius - $r1);
					}
					$iteration++;
				}
				$output[$y][$x] = $colorSoFar;
			}
		}
	}
    
}

qb_compile();

$image = imagecreatefrompng("../ipad.png");
$output = imagecreatetruecolor(imagesx($image), imagesy($image));

$droste = new Droste;
$droste->generate($output, $image);
imagesavealpha($output, true);
imagepng($output, "test.png");

?>