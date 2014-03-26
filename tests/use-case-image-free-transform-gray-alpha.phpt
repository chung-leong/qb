--TEST--
Use case: Image free transform (grayscale with alpha)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path = "$folder/output/image-free-transform-gray-alpha.correct.png";
$incorrect_path = "$folder/output/image-free-transform-gray-alpha.incorrect.png";

/** @engine qb
  * @param  image2        $dst
  * @param  image2        $src
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
function free_transform(&$dst, $src, $A, $B, $C, $D) {
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

$width = imagesx($image);
$height = imagesy($image);
$output = imagecreatetruecolor($width, $height);

$A = array(150, 10);
$B = array($width - 80, 150);
$C = array($width - 100, $height - 100);
$D = array(0, $height - 50);

free_transform($output, $image, $A, $B, $C, $D);

ob_start();
imagesavealpha($output, true);
imagepng($output);
$output_png = ob_get_clean();

/**
 * @engine qb
 *
 * @param image	$img2;
 * @param image	$img1;
 * @return float32
 */
function _image_diff($img1, $img2) {
	$img2 -= $img1;
	$img2 *= $img2;
	return sqrt(array_sum($img2));
}

if(file_exists($correct_path)) {
	$correct_md5 = md5_file($correct_path);
	$output_md5 = md5($output_png);
	if($correct_md5 == $output_md5) {
		// exact match
		$match = true;
	} else {
		$correct_output = imagecreatefrompng($correct_path);
		$diff = _image_diff($output, $correct_output);
		if($diff < 1.5) {
			// the output is different ever so slightly
			$match = true;
		} else {
			$match = false;
		}
	}
	if($match) {
		echo "CORRECT\n";
		if(file_exists($incorrect_path)) {
			unlink($incorrect_path);
		}
	} else {
		echo "INCORRECT (diff = $diff)\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
	echo "CORRECT\n";
}

?>
--EXPECT--
CORRECT
