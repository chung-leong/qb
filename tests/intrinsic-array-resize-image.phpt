--TEST--
Array resize test (image)
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[?][?][4]	$image
 * @local	uint32			$(width|height)
 * 
 * @return	void
 * 
 */
function test_function(&$image) {
	$height = count($image);
	$width = count($image[0]);
	
	array_resize($image, $height * 0.75, $width * 1.50);
	
	$height = count($image);
	$width = count($image[0]);
	echo "{$width}x{$height}\n";
}

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path = "$folder/output/image-resize.correct.png";
$incorrect_path = "$folder/output/image-resize.incorrect.png";
test_function($image);

ob_start();
imagesavealpha($image, true);
imagepng($image);
$output_png = ob_get_clean();

if(file_exists($correct_path)) {
	$correct_md5 = md5_file($correct_path);
	$output_md5 = md5($output_png);
	if($correct_md5 == $output_md5) {
		$match = true;
	} else {
		$match = false;
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
768x288
CORRECT
