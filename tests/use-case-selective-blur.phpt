--TEST--
Use case: Selective blur
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
$output = imagecreatetruecolor(imagesx($image), imagesy($image));
$correct_path = "$folder/output/image-selective-blur.correct.png";
$incorrect_path = "$folder/output/image-selective-blur.incorrect.png";

/**
 * @engine qb
 * @param image				$output
 * @param image				$image
 *
 * @local uint32			$(width|height)
 * @local uint32			$(ix|iy)
 * @local float32			$(x|y|dx|dy)
 * @local float32[7][7]		$gaussian
 */
function blur(&$output, $image) {
	$height = count($output);
	$width = count($output[0]);
	$gaussian = array(
		array(0.00000067,	0.00002292,	0.00019117,	0.00038771,	0.00019117,	0.00002292,	0.00000067),
		array(0.00002292,	0.00078634,	0.00655965,	0.01330373,	0.00655965,	0.00078633,	0.00002292),
		array(0.00019117,	0.00655965,	0.05472157,	0.11098164,	0.05472157,	0.00655965,	0.00019117),
		array(0.00038771,	0.01330373,	0.11098164,	0.22508352,	0.11098164,	0.01330373,	0.00038771),
		array(0.00019117,	0.00655965,	0.05472157,	0.11098164,	0.05472157,	0.00655965,	0.00019117),
		array(0.00002292,	0.00078633,	0.00655965,	0.01330373,	0.00655965,	0.00078633,	0.00002292),
		array(0.00000067,	0.00002292,	0.00019117,	0.00038771,	0.00019117,	0.00002292,	0.00000067)
	);
	
	for($iy = 0, $y = 0.5; $iy < $height; $iy++, $y++) {
		for($ix = 0, $x = 0.5; $ix < $width; $ix++, $x++) {
			$dx = $x - 340;
			$dy = $y - 110;
			if(hypot($dx, $dy) < 80) {
				$output[$iy][$ix] = sample_convolution($image, 340 + $dx * 0.5, 110 + $dy * 0.5, $gaussian, 1, 0.2);
			} else {
				$output[$iy][$ix] = sample_nearest($image, $x, $y);
			}
		}
	}
}

blur($output, $image);

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
