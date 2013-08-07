--TEST--
Sampling test (nearest-neighbor)
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
	$pixel = sample_nearest($image, 10.2, 8.8);
	echo "$pixel\n";
	
	$x_positions = array(1, 2, 3, 4);
	$y_position = 1;
	$pixels = sample_nearest($image, $x_positions, $y_position);
	echo "$pixels\n";
}

qb_compile();
ini_set('precision', 10);
$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
test_function($image);

?>
--EXPECT--
[0.10588, 0.10588, 0.10588, 1]
[[0.05098, 0.031373, 0.054902, 1], [0.058824, 0.039216, 0.062745, 1], [0.062745, 0.043137, 0.066667, 1], [0.066667, 0.047059, 0.070588, 1]]
