<?php

/**
  * @engine qb
  * @param float32[][4]			$image
  *
  * @local float32[4][4]		$YCbCrMatrix
  * @local float32[r,g,b,a]		$pixel
  * @local float32[y,cb,cr,a]	$ycbcr
  * @local uint32				$index
  * @local float32				$count
  *
  * @return float32
  */
function get_skin_color_pixel_ratio(&$image) {
	$YCbCrMatrix = array(
		array(	0.299,	-0.169,	0.500,	0.000	),
		array(	0.587,	-0.331,	-0.419,	0.000	),
		array(	0.114,	0.500,	-0.081,	0.000	),
		array(	0.000,	0.500,	0.500,	1.000	),
	);
	foreach($image as $index => $pixel) {
		$ycbcr = mv_mult($YCbCrMatrix, $pixel);
		if($ycbcr->cb > 0.412 && $ycbcr->cb < 0.530 && $ycbcr->cr > 0.550 && $ycbcr->cr < 0.647) {
			$image[$index] = array(0, 1, 0, 1);
			$count++;
		}
	}
	return $count / count($image);
}

qb_compile();

$folder = dirname(__FILE__);

for($i = 1; $i <= 5; $i++) {
	$image = imagecreatefrompng("$folder/image$i.png");
	$skin = get_skin_color_pixel_ratio($image);
	echo $skin, "<br>";
	imagepng($image, "$folder/thumbnail_skin_green$i.png");
}

?>