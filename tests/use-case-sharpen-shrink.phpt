--TEST--
Use case: Sharpen and shrink
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$output = imagecreatetruecolor(imagesx($image) / 2, imagesy($image) / 2);
$correct_path = "$folder/output/image-sharpen-shrink.correct.png";
$incorrect_path = "$folder/output/image-sharpen-shrink.incorrect.png";

/**
 * @engine qb
 * @param image				$output
 * @param image				$image
 *
 * @local uint32			$(width|height)
 * @local uint32			$(ix|iy)
 * @local float32			$(x|y|divisor)
 * @local float32[3][3]		$sharpenMatrix
 */
function sharpen(&$output, $image) {
	$height = count($output);
	$width = count($output[0]);
	$sharpenMatrix = array(
		array(-1.2, -1, -1.2), 
		array(-1, 20, -1), 
		array(-1.2, -1, -1.2)
	);
	$divisor = array_sum($sharpenMatrix);
	
	for($iy = 0, $y = 1; $iy < $height; $iy++, $y += 2) {
		for($ix = 0, $x = 1; $ix < $width; $ix++, $x += 2) {
			$output[$iy][$ix] = sample_convolution($image, $x, $y, $sharpenMatrix, $divisor, 0);
		}
	}
}

sharpen($output, $image);

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
		if($diff < 0.01) {
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
	echo "CORRECT\n";
}

?>
--EXPECT--
CORRECT
