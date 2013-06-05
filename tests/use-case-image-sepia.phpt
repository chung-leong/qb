--TEST--
Use case: Sepia
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path = "$folder/output/sepia-2.correct.png";
$incorrect_path = "$folder/output/sepia-2.incorrect.png";

/**
 * @engine qb
 * @param image			$image
 * @param float32		$intensity
 *
 * @local float32[4][4]	$YIQMatrix
 * @local float32[4][4]	$inverseYIQ
 * @local float32[4]	$k
 */
function sepia(&$image, $intensity) {
	$YIQMatrix = array(
		array(0.299,  0.596,  0.212, 0.000),
		array(0.587, -0.275, -0.523, 0.000),
		array(0.114, -0.321,  0.311, 0.000),
		array(0.000,  0.000,  0.000, 1.000),
	);
	
	$image = mv_mult($YIQMatrix, $image);
	$k = array(1, 0, 0, 1);
	$image *= $k;		// clear I and Q
	$k = array(0, $intensity, 0, 0);
	$image += $k;		// set I to intensity
	$inverseYIQ = array(
		array(1.0,    1.0,    1.0,    0.0),
		array(0.956, -0.272, -1.10,   0.0),
		array(0.621, -0.647,  1.70,   0.0),
		array(0.0,    0.0,    0.0,    1.0),
	);	
	$image = mv_mult($inverseYIQ, $image);
}

qb_compile();

sepia($image, 0.2);

ob_start();
imagesavealpha($image, true);
imagepng($image);
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
		echo "INCORRECT ($diff)\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
}

?>
--EXPECT--
CORRECT