--TEST--
Blend test
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$correct_path_templ = "$folder/output/blend#.correct.png";
$incorrect_path_templ = "$folder/output/blend#.incorrect.png";

/**
 * @engine qb
 * @param image		$dst
 * @param image		$image1
 */
function blend1(&$dst, $image1) {
	$dst = blend($dst, $image1);
}

/**
 * @engine qb
 * @param image		$dst
 * @param image		$image1
 * @param image		$image2
 */
function blend2(&$dst, $image1, $image2) {
	$dst = blend($image1, $image2);
}

/**
 * @engine qb
 * @param image2	$dst
 * @param image2	$image1
 * @param image2	$image2
 */
function blend3(&$dst, $image1, $image2) {
	$dst = blend($image1, $image2);
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

for($i = 1; $i <= 3; $i++) {
	$correct_path = preg_replace('/#/', $i, $correct_path_templ);
	$incorrect_path = preg_replace('/#/', $i, $incorrect_path_templ);
	
	switch($i) {
		case 1:
			$red = imagecreatefrompng("$folder/input/red-circle.png");
			$blue = imagecreatefrompng("$folder/input/blue-circle.png");
			blend1($red, $blue);
			$image = $red;
			break;
		case 2: 
			$red = imagecreatefrompng("$folder/input/red-circle.png");
			$blue = imagecreatefrompng("$folder/input/blue-circle.png");
			$image = imagecreatetruecolor(imagesx($red), imagesy($red));
			imagesavealpha($image, true);
			blend2($image, $blue, $red);
			break;
		case 3: 
			$red = imagecreatefrompng("$folder/input/red-circle.png");
			$blue = imagecreatefrompng("$folder/input/blue-circle.png");
			$image = imagecreatetruecolor(imagesx($red), imagesy($red));
			imagesavealpha($image, true);
			blend3($image, $blue, $red);
			break;
	}

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
