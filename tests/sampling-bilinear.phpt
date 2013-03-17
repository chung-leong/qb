--TEST--
Sampling test (bilinear)
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

qb_compile();

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
test_function($image);

?>
--EXPECTREGEX--
\[0\.098235\d+, 0\.08729411, 0\.0(9|89+), 1\]
\[\[0\.05490\d+, 0\.03529412, 0\.05882353, 1\], \[0\.05784314, 0\.0382353, 0\.06176471, 1\], \[0\.064705\d+, 0\.04509804, 0\.06764706, 1\], \[0\.06764706, 0\.04803922, 0\.07058824, 1\]\]
