--TEST--
Use case: Green screen
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/helicopter.png");
$correct_path = "$folder/output/green-screen.correct.png";
$incorrect_path = "$folder/output/green-screen.incorrect.png";

/**
 * @engine qb
 * @param float32[][][4]    $image
 * @param float32           $strength
 * @return void
 *
 * @local uint32			$(x|y|width|height)
 * @local float32[y,i,q,a]	$pixel
 * @local float32[4][4]		$(m1|m2)
 */
function green_screen(&$image, $strength = 4) {
	$height = count($image);
	$width = count($image[0]);
	$m1 = array(
		array(0.299,    0.587,     0.114,     0.0),
		array(0.595716, -0.274453, -0.321263, 0.0),
		array(0.211456, -0.522591, 0.31135,   0.0),
		array(0.0,      0.0,       0.0,       1.0),
	);
	$image = mv_mult($m1, $image);
	for($y = 0; $y < $height; $y++) {
		for($x = 0; $x < $width; $x++) {
			$pixel = $image[$y][$x];
			if($pixel->i < 0 && $pixel->q < 0 && $pixel->y > 0.01) {
				$pixel->a = 1.0 - hypot($pixel->i, $pixel->q) * $pixel->y * $strength;
				$pixel->i = $pixel->q = 0;
			}
			$image[$y][$x] = $pixel;
		}
	}
	$m2 = inverse($m1);
	$image = mv_mult($m2, $image);
}

green_screen($image);

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
		$diff = _image_diff($image, $correct_output);
		if($diff < 2) {
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

