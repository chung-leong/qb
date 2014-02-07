--TEST--
Image array test 
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image1 = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$image2 = imagecreatefrompng("$folder/input/malgorzata_socha2.png");
$image3 = imagecreatefrompng("$folder/input/joanna-krupa.png");
$correct_path = "$folder/output/image-array.correct.png";
$incorrect_path = "$folder/output/image-array.incorrect.png";

$max_width = max(imagesx($image1), imagesx($image2), imagesx($image3));
$max_height = max(imagesy($image1), imagesy($image2), imagesy($image3));
$output = imagecreatetruecolor($max_width, $max_height);

$images = array($image1, $image2, $image3);

/**
 * @engine qb
 * @param image				$output
 * @param image[]			$images
 * @local float32[r,g,b,a]	$pixel
 * @local uint32			$(i|x|y|width|height)
 *
 */
function merge(&$output, $images) {
	$height = count($output);
	$width = count($output[0]);

	for($i = 0; $i < count($images); $i++) {
		for($y = 0; $y < $height; $y++) {
			for($x = 0; $x < $width; $x++) {
				$pixel = sample_nearest($images[$i], $x, $y);
				$pixel->a *= 0.5;
				$output[$y][$x] = blend($output[$y][$x], $pixel);
			}
		}
	}
}

merge($output, $images);

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
	$img2 *= $img2;
	return sqrt(array_sum($img2));
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
		if($diff < 3.5) {
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

?>
--EXPECT--
CORRECT
