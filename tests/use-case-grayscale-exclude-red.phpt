--TEST--
Use case: Gray scale exclude red 
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/joanna-krupa.png");
$correct_path = "$folder/output/grayscale-red.correct.png";
$incorrect_path = "$folder/output/grayscale-red.incorrect.png";

/**
 * @engine qb
 * @param float32[][][4]  $image
 * @return void
 *
 * @local uint32      $(x|y|width|height)
 * @local float32[h,s,l,a]  $pixel
 * @local float32      $hue
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

grayscale_exclude_red($image);

ob_start();
imagesavealpha($image, true);
imagepng($image);
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
		$diff = _image_diff($image, $correct_output);
		if($diff < 3.5) {
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
