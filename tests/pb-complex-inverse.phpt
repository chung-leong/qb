--TEST--
Pixel Bender test: Complex Mapping z = d/[(z-a)(z-b)(z-c)]
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$filter_name = "complex-inverse";
$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$output = imagecreatetruecolor(imagesx($image), imagesy($image));
$correct_path = "$folder/output/$filter_name.correct.png";
$incorrect_path = "$folder/output/$filter_name.incorrect.png";

/**
 * @engine qb
 * @import pbj/complex-inverse.pbj
 *
 * @param image			$dst
 * @param image			$src
 * @param float32[2]	$a
 * @param float32[2]	$b
 * @param float32[2]	$c
 * @param float32[2]	$d
 * @param float32[2]	$distort
 * @param float32[2]	$imagesize
 * @param float32[2]	$center
 * @param float32		$focus
 * @param float32		$scale
 * @param float32		$fill
 */
function filter(&$dst, $src, $a, $b, $c, $d, $distort, $imagesize, $center, $focus, $scale, $fill) {}

qb_compile();

filter($output, $image, array(-0.2, -0.4), array(-0.7, -1.1), array(0, 0.1), array(2, -1.6), array(20, 20), array(500, 225), array(420, 420), 0, 300, 0.5);

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
	return abs(array_sum($img2));;
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
		if(abs($diff) < 0.1) {
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
		echo "INCORRECT $diff\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
}


?>
--EXPECT--
CORRECT