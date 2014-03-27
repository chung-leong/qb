--TEST--
Use case: Image reflection (using fork)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha2.png");
$correct_path = "$folder/output/image-reflection.correct.png";
$incorrect_path = "$folder/output/image-reflection-fork.incorrect.png";
$output = imagecreatetruecolor(640, 480);

class ReflectionFilter {

	/** @var float32    From the top, where the image should sit (fraction of the canvas height) */
	protected $bottom;
	
	/** @var float32    Width of the gap between the image and its reflection (fraction of the canvas height) */
	protected $gap;
	
	/** @var float32    Scaling factor for the image */
	protected $scale;
	
	/** @var float32    How strongly the surface reflect the image */
	protected $reflectivity;
	
	/** @var float32    The length of the reflection (fraction of the image height) */
	protected $length;
		
	public function __construct($bottom = 0.75, $gap = 0.02, $scale = 0.6, $reflectivity = 0.5, $length = 0.5) {
		$this->bottom = $bottom;
		$this->gap = $gap;
		$this->scale = $scale;
		$this->reflectivity = $reflectivity;
		$this->length = $length;
	}
	
	public function __get($name) {
		switch($name) {
			case 'bottom':	return $this->bottom;
			case 'gap':	return $this->gap;
			case 'scale': return $this->scale;
			case 'reflectivity': return $this->reflectivity;
			case 'length': return $this->length;
		}
	}

	public function __set($name, $value) {
		switch($name) {
			case 'bottom': $this->bottom = (double) $value; break;
			case 'gap': $this->gap = (double) $value; break;
			case 'scale': $this->scale = (double) $value; break;
			case 'reflectivity': $this->reflectivity = (double) $value; break;
			case 'length': $this->length = (double) $value; break;
		}
	}

	/**
	  * @engine qb
	  * @param image $canvas     The image to draw into
	  * @param image $image      The image to copy from
	  *
	  * @local uint32 $(canvas_.*)
	  * @local uint32 $(dest_.*)
	  * @local uint32 $(refl_.*)
	  * @local float32 $(image_.*)
	  * @local float32[r,g,b,a] $(pixel|sample)
	  * @local float32 $k;
	  * @local uint32 $fork_id
	  */
	public function draw(&$canvas, $image) {
		$canvas_height = count($canvas);
		$canvas_width = count($canvas[0]);
		$image_height = count($image);
		$image_width = count($image[0]);
		$image_adv = 1 / $this->scale;
		$dest_height = round($image_height * $this->scale);
		$dest_width = round($image_width * $this->scale);
		$dest_left = ($canvas_width - $dest_width) / 2;
		$dest_right = $dest_left + $dest_width;
		$dest_bottom = $canvas_height * $this->bottom;
		$dest_top = $dest_bottom - $dest_height;
		$refl_top = $dest_bottom + ($canvas_height * $this->gap);
		$refl_bottom = $refl_top + $dest_height;

		$image_refl_height = $image_height * $this->length;
		$image_refl_start = $image_height - $image_refl_height;
		
		$fork_id = fork($dest_bottom - $dest_top);
		$dest_y = $dest_top + $fork_id;
		$refl_y = $refl_bottom - $fork_id;
		$image_y = $fork_id * $image_adv;

		$k = ($image_y - $image_refl_start) / $image_refl_height * $this->reflectivity;
		for($dest_x = $dest_left, $image_x = 0; $dest_x < $dest_right; $dest_x++, $image_x += $image_adv) {
			$pixel = $canvas[$dest_y][$dest_x];
			$sample = sample_bilinear($image, $image_x, $image_y);
			$pixel = mix($pixel, $sample, $sample->a);
			$canvas[$dest_y][$dest_x] = $pixel;
			
			if($k > 0.0 && $refl_y < $canvas_height) {
				$pixel = $canvas[$refl_y][$dest_x];
				$pixel = mix($pixel, $sample, $sample->a * $k);
				$canvas[$refl_y][$dest_x] = $pixel;
			}
		}
	}
}

$background_color = imagecolorallocate($output, 0, 0, 0);
imagefilledrectangle($output, 0, 0, imagesx($output), imagesy($output), $background_color);
$filter = new ReflectionFilter();
$filter->draw($output, $image);

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
		if($diff < 2) {
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

