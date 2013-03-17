--TEST--
Pixel Bender test: Cubic Space
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$filter_name = "cubic-space";
$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$output = imagecreatetruecolor(imagesx($image), imagesy($image));
$correct_path = "$folder/output/$filter_name.correct.png";
$incorrect_path = "$folder/output/$filter_name.incorrect.png";

/**
 * @engine qb
 * @import pbj/cubic-space.pbj
 *
 * @param image			$dst
 * @param image			$src
 */
function filter(&$dst, $src) {
}

qb_compile();

filter($output, $image);

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