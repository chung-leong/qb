<?php

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

qb_compile();

chdir(dirname(__FILE__));
$img = imagecreatefrompng("input.png");
green_screen($img);
imagesavealpha($img, true);
imagepng($img, "output.png");
echo "Done";

?>