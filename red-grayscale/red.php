<?php

/**
 * @engine qb
 * @param float32[][][4]	$image
 * @return void
 *
 * @local uint32			$(x|y|width|height)
 * @local float32[h,s,l,a]	$pixel
 * @local float32			$hue
 */
function grayscale_exclude_red(&$image) {
	$height = count($image);
	$width = count($image[0]);
	
	// convert RGB values to HSL
	$image = rgb2hsl($image);
	
	for($y = 0; $y < $height; $y++) {
		for($x = 0; $x < $width; $x++) {
			$pixel = $image[$y][$x];
			
			// account for hue > 345
			$hue = ($pixel->h > 345) ? 360 - $pixel->h : $pixel->h;
			
			// adjust the saturation
			$pixel->s *= (1 - smooth_step(2.5, 15, $hue)) * smooth_step(0.2, 0.9, $pixel->s);
			
			$image[$y][$x] = $pixel;
		}
	}
	
	// convert HSL back to RGB
	$image = hsl2rgb($image);
}

$folder = dirname(__FILE__);
$img = imagecreatefromjpeg("$folder/joanna-krupa.jpg");

grayscale_exclude_red($img);

header("Content-Type: image/jpeg");
imagejpeg($img, NULL, 90);

?>