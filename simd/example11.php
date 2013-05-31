<?php

/**
 * @engine qb
 * @param image4     $image
 * @local float32[4] $a
 */
function test(&$image) {
	$a = array(1.0, 1.0, 1.0, 0.5);
	$image *= $a;
}

qb_compile();

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/socha.png"); 
test($image);
imagesavealpha($image, true);
imagepng($image, "$folder/socha-out.png");

?>