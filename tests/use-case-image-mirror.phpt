--TEST--
Use case: Image mirror
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path = "$folder/output/image-mirror.correct.png";
$incorrect_path = "$folder/output/image-mirror.incorrect.png";

/**
 * @engine qb
 * @param image		$image
 * @local uint32	$i
 */
function mirror(&$image) {
	for($i = 0; $i < count($image); $i++) {
		$image[$i] = array_reverse($image[$i]);
	}
}

qb_compile();

mirror($image);

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
}

?>
--EXPECT--
CORRECT