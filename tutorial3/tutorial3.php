<?php

/**
 * @engine qb
 * @param float32[][][4]	$image
 * @return void
 *
 * @local uint32			$(x|y|width|height)
 * @local float32[y,i,q,a]	$pixel
 * @local float32[4][4]		$(m1|m2)
 */
function green_screen(&$image) {
	$m1 = array(
		array(0.299,  0.596,  0.212, 0.000),
		array(0.587, -0.275, -0.523, 0.000),
		array(0.114, -0.321,  0.311, 0.000),
		array(0.000,  0.000,  0.000, 1.000),
	);
	$m2 = inverse($m1);
	$height = count($image);
	$width = count($image[0]);
	$image = mv_mult($m1, $image);
	for($y = 0; $y < $height; $y++) {
		for($x = 0; $x < $width; $x++) {
			$pixel = $image[$y][$x];
			if($pixel->i < 0 && $pixel->q < 0 && $pixel->y > 0.05) {
				$pixel->a = 1.0 - hypot($pixel->i, $pixel->q) * $pixel->y * 3;
				$pixel->i = $pixel->q = 0;

			}
			$image[$y][$x] = $pixel;
		}
	}
	$image = mv_mult($m2, $image);
}

qb_compile();

chdir(dirname(__FILE__));
$img = imagecreatefrompng("input.png");
//$img = imagecreatefromjpeg("joker_input.jpg");
green_screen($img);
imagesavealpha($img, true);
imagepng($img, "output.png");
echo "Done";

?>