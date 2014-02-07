--TEST--
Yield test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5.0') < 0) print 'skip feature not available';
	if(!function_exists('imagecreatetruecolor')) print 'skip GD functions not available';
?>
--FILE--
<?php

/**
 * @engine qb
 * @param image			$(dst|src)
 * @param float32		$intensity
 *
 * @local float32[4][4]	$YIQMatrix
 * @local float32[4][4]	$inverseYIQ
 * @local float32[4]	$k
 */
function sepia(&$dst, $src, $intensity) {
	ini_set("qb.column_major_matrix", true);
	
	$YIQMatrix = array(
		array(0.299,  0.596,  0.212, 0.000),
		array(0.587, -0.275, -0.523, 0.000),
		array(0.114, -0.321,  0.311, 0.000),
		array(0.000,  0.000,  0.000, 1.000),
	);
	
	$src = mv_mult($YIQMatrix, $src);
	$src *= array(1, 0, 0, 1);				// clear I and Q
	$src += array(0, $intensity, 0, 0);		// set I to intensity
	$inverseYIQ = array(
		array(1.0,    1.0,    1.0,    0.0),
		array(0.956, -0.272, -1.10,   0.0),
		array(0.621, -0.647,  1.70,   0.0),
		array(0.0,    0.0,    0.0,    1.0),
	);
	$dst = mv_mult($inverseYIQ, $src);
	
	ini_restore("qb.column_major_matrix");
}

/**
 * @engine	qb
 * @param	image	$src
 * @local	image	$dst
 * @local	float32	$intensity
 * @return	image
 * 
 */
function yield_sepia($src) {
	$intensity = 0;
	while($intensity < 0.5) {
		$intensity += 0.1;
		sepia($dst, $src, $intensity);
		yield $dst;
	}
}

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

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path_templ = "$folder/output/yield-sepia-#.correct.png";
$incorrect_path_templ = "$folder/output/yield-sepia-#.incorrect.png";

foreach(yield_sepia($image) as $index => $image) {
	$correct_path = preg_replace('/#/', $index + 1, $correct_path_templ);
	$incorrect_path = preg_replace('/#/', $index + 1, $incorrect_path_templ);
	
	ob_start();
	imagesavealpha($image, true);
	imagepng($image);
	$output_png = ob_get_clean();
	
	if(file_exists($correct_path)) {
		$correct_md5 = md5_file($correct_path);
		$output_md5 = md5($output_png);
		if($correct_md5 == $output_md5) {
			// exact match
			$match = true;
		} else {
			$correct_output = imagecreatefrompng($correct_path);
			$diff = _image_diff($image, $correct_output);
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
}

?>
--EXPECT--
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
