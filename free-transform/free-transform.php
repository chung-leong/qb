<?php

/** @engine qb
  * @param  image         $dst
  * @param  image         $src
  * @param  float32[x,y]  $A
  * @param  float32[x,y]  $B
  * @param  float32[x,y]  $C
  * @param  float32[x,y]  $D
  *
  * @local  float32[x,y]  $(AB|BC|CD|DA|AP|BP|CP|DP)
  * @local  float32[x,y]  $P
  * @local  float32       $(dab|dbc|dcd|dda)
  * @local	float32	      $(srcX|srcY|srcWidth|srcHeight)
  * @local	uint32	      $(dstX|dstY|dstWidth|dstHeight)
  * @local  float32[4]    $pixel
*/  
function free_transform(&$dst, &$src, $A, $B, $C, $D) {
	$dstWidth = count($dst[0]);
	$dstHeight = count($dst);
	
	$srcWidth = count($src[0]);
	$srcHeight = count($src);
	
	$AB = $A - $B;
	$BC = $B - $C;
	$CD = $C - $D;
	$DA = $D - $A;

	for($dstY = 0, $P->y = 0; $dstY < $dstHeight; $dstY++, $P->y++) {
		for($dstX = 0, $P->x = 0; $dstX < $dstWidth; $dstX++, $P->x++) {

			$AP = $A - $P;
			$BP = $B - $P;
			$CP = $C - $P;
			$DP = $D - $P;
			
			$dab = $AP->x * $AB->y - $AP->y * $AB->x;
			$dbc = $BP->x * $BC->y - $BP->y * $BC->x;
			$dcd = $CP->x * $CD->y - $CP->y * $CD->x;
			$dda = $DP->x * $DA->y - $DP->y * $DA->x;
			
			$srcX = $srcWidth * $dda / ($dda + $dbc);
			$srcY = $srcHeight * $dab / ($dab + $dcd);
		
			$dst[$dstY][$dstX] = sample_bilinear($src, $srcX, $srcY);
		}
	}
}

$folder =  dirname(__FILE__);
$src = imagecreatefromjpeg("$folder/socha.jpg");
$width = imagesx($src);
$height = imagesy($src);
$output = imagecreatetruecolor($width, $height);

$A = array(150, 10);
$B = array($width - 80, 150);
$C = array($width - 100, $height - 100);
$D = array(0, $height - 50);

free_transform($output, $src, $A, $B, $C, $D);

header("Content-Type: image/png");
imagesavealpha($output, true);
imagepng($output);

?>