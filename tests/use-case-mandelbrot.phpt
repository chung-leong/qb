--TEST--
Use case: Mandelbrot fractal 
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

// Ported from Pixel Bender
// http://www.subblue.com/blog/2008/11/13/fast_mandelbrot_renderer

/**
* Mandelbrot.pbk
* 
* Copyright (c) 2008 Tom Beddard
* http://www.subblue.com
* 
* Licensed under the MIT License: http://www.opensource.org/licenses/mit-license.php
* 
* 
* Tips:
* - Use the centerPreset option to jump to interesting parts of the Mandelbrot set.
* - Increase the maxIterations option to increase detail on high zooms.
* 
* As loops aren't supported with Hydra code it curently isn't possible to export this
* for use in Flash. 
*/

class Mandelbrot {

	/** @var float[x,y] */
	public $center = array(-0.5, 0.0);

	/** @var float[x,y] */
	public $centerOffset = array(0.0, 0.0);

	/** @var int */
	public $centerPreset = 0;

	/** @var float */
	public $zoomMajor = 0;

	/** @var float */
	public $zoomMinor = 0;

	/** @var int */
	public $maxIterations = 200;
	
	/** @var float */
	protected $width;
	
	/** @var float */
	protected $height;

	const BAILOUT = 10.0;
	
	/**
	 * Calculate the output colour for each input pixel
	 *
	 * @engine qb
	 *
	 * @param float[x,y]		$coord
	 *
	 * @local float[1]			$pixel
	 * @local float				$(aa|bb|zz|twoab)
	 * @local float				$(x0|y0|x1|y1|x2|y2)
	 * @local float				$(zoom|spanX|spanY)
	 * @local float[x,y]		$(z|z0)
	 * @local int				$n
	 * 
	 * @return float[1]
	 */
	protected function renderPixel($coord)
	{
		// Use a center preset to find some nice parts of the Mandelbrot set
		switch ($this->centerPreset) {
			case 1:
				$x0 = -0.742522478103764;
				$y0 = -0.143708014488453;
				break;
			case 2:
				$x0 = 0.36295341867850555;
				$y0 = -0.6455617463848476;
				break;
			case 3:
				$x0 = 0.3218759918211005;
				$y0 = 0.03518083572368085;
				break;
			case 4:
				$x0 = -1.673497088962531;
				$y0 = -0.0003318667941149705;
				break;
			default:
				$x0 = $this->center->x;
				$y0 = $this->center->y;		
		}

		$zoom = exp($this->zoomMajor + $this->zoomMinor);
		$x1 = $x0 - 2.0 / $zoom;	 // Left limit of x
		$x2 = $x0 + 2.0 / $zoom;	 // Right limit of x

		$spanX = $x2 - $x1;
		$spanY = $spanX * ($this->height / $this->width);
		$y1 = $y0 - $spanY / 2.0;
		$y2 = $y0 + $spanY / 2.0;

		$x1 += $this->centerOffset->x * $spanX; // Shift centre for fine tuning of position
		$y1 += $this->centerOffset->y * $spanY;

		$z->x = $x1 + $coord->x * $spanX / $this->width;
		$z->y = $y1 + $coord->y * $spanY / $this->height;
		$z0 = $z;
		$n = 0;

		/**
		* Mandelbrot formula
		*	z' = z^2 + c
		*	 where: z = a + bi
		*	 z^2 = (a^2 - b^2) + (2ab)i
		*	 and: c = x + yi
		*/
		while ($n < $this->maxIterations) {
			$aa = $z->x * $z->x;
			$bb = $z->y * $z->y;

			if ($aa + $bb > self::BAILOUT) {
				// Fill the pixel based on the time it takes to reach the bailout threshold
				$pixel[0] = 1.0 - $n / $this->maxIterations;
				break;
			}

			$twoab = 2.0 * $z->x * $z->y;
			$z->x = $aa - $bb + $z0->x;
			$z->y = $twoab + $z0->y;
			$n++;
		}
		
		return $pixel;
	}
	
	/**
	 *
	 * @engine qb
	 *
	 * @param image1		$dst
	 *
	 * @local int			$width
	 * @local int			$height
	 * @local int			$x
	 * @local int			$y
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
		
		$y = fork($height);
		$coord->y = $y + 0.5;
		for ($x = 0, $coord->x = 0.5; $x < $width; $x++, $coord->x++) {
			$dst[$y][$x] = $this->renderPixel($coord);
		}
	}
}

$folder = dirname(__FILE__);
$output = imagecreatetruecolor(400, 300);
$correct_path = "$folder/output/mandelbrot.correct.png";
$incorrect_path = "$folder/output/mandelbrot.incorrect.png";

$renderer = new Mandelbrot;
$renderer->centerPreset = 3;
$renderer->zoomMajor = 5;
$renderer->generate($output);

ob_start();
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
