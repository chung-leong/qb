<?php

/**
 * @engine qb
 * @param float32[][][4]	$image
 * @return void
 *
 * @local uint32			$(x|y|width|height)
 * @local float32[4]		$pixel
 */
function green_screen(&$image) {
	$height = count($image);
	$width = count($image[0]);
	for($y = 0; $y < $height; $y++) {
		for($x = 0; $x < $width; $x++) {
			$pixel = $image[$y][$x];
			if($pixel == array(0.0, 1.0, 0.0, 1.0)) {
				$pixel[3] = 0.0;
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