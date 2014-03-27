--TEST--
Sampling test (bilinear)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

/**
 * @engine qb
 * @param image				$image
 *
 * @local float32[4]		$pixel
 * @local float32[4]		$x_positions
 * @local float32			$y_position
 * @local float32[4][4]		$pixels
 */
function test_function($image) {
	$pixel = sample_bilinear($image, 10.2, 8.8);
	echo "$pixel\n";
	
	$x_positions = array(1, 2, 3, 4);
	$y_position = 1;
	$pixels = sample_bilinear($image, $x_positions, $y_position);
	echo "$pixels\n";
}

ini_set('precision', 10);
$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
test_function($image);

?>
--EXPECT--
[0.098235, 0.087294, 0.09, 1]
[[0.054902, 0.035294, 0.058824, 1], [0.057843, 0.038235, 0.061765, 1], [0.064706, 0.045098, 0.067647, 1], [0.067647, 0.048039, 0.070588, 1]]