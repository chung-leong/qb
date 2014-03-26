--TEST--
Multiple sampling test
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagecreatetruecolor')) print 'skip GD functions not available';
?>
--FILE--
<?php

/**
 * @engine qb
 * @param float32[][][4] $a
 * @param float32[]	$(x|y)
 * @local float32[][4]	$r
 * @local float64 $(s|e)
 */
function test_function($a, $x, $y) {
	$r = sample_bilinear($a, $x, $y);
	echo $r, "\n";
}

$width = 256;
$height = 256;
$image = imagecreatetruecolor($width, $height);
$color = imagecolorallocate($image, 255, 255, 255);
imagefilledrectangle($image, 0, 0, $width, $height, $color);
$color = imagecolorallocatealpha($image, 255, 0, 0, 64);
imagefilledrectangle($image, 0, 0, $width / 2, $height / 2, $color);

$x = array(0, 32, $width / 2 + 4, $width);
$y = array(0, 32, $height);

test_function($image, $x, $y)

?>
--EXPECT--
[[0.25, 0.1254902, 0.1254902, 0.25], [1, 0.5019608, 0.5019608, 1], [0.5, 0.5, 0.5, 0.5], [0.25, 0.25, 0.25, 0.25]]

