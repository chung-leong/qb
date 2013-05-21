<?php

/**
 * @engine qb
 * @param float32[][][4]	$image
 * @return void
 *
 * @local uint32			$(x|y|width|height)
 * @local float32[r,g,b,a]	$pixel
 */
function green_screen(&$image) {
	$height = count($image);
	$width = count($image[0]);
	for($y = 0; $y < $height; $y++) {
		for($x = 0; $x < $width; $x++) {
			$pixel = $image[$y][$x];
//			if($pixel[1] >= 0.8 && $pixel[0] < 0.2 && $pixel[2] < 0.2) {
			if($pixel->g >= 0.8 && $pixel->r < 0.2 && $pixel->b < 0.2) {
				$pixel->a = 0.0;
			}
			$image[$y][$x] = $pixel;
		}
	}
}

qb_compile();

chdir(dirname(__FILE__));
$img = imagecreatefrompng("input.png");
green_screen($img);
imagesavealpha($img, true);
imagepng($img, "output.png");
echo "Done";

?>