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

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
test_function($image);

?>
--EXPECT--
[0.1058824, 0.1058824, 0.1058824, 1]
[[0.05098039, 0.03137255, 0.05490196, 1], [0.05882353, 0.03921569, 0.0627451, 1], [0.0627451, 0.04313726, 0.06666667, 1], [0.06666667, 0.04705882, 0.07058824, 1]]
