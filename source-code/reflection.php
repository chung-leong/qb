<?php

$start = microtime(true);
$folder = dirname(__FILE__);
$image = imagecreatetruecolor(852, 480);
$canvas = imagecreatetruecolor(800, 600);
$file_in = av_file_open("$folder/source-code.mov", "r");
$file_out1 = av_file_open("$folder/source-code-reflection.webm", "w");
$file_out2 = av_file_open("$folder/source-code-reflection.mp4", "w");

$v_strm_in = av_stream_open($file_in, "video");
$a_strm_in = av_stream_open($file_in, "audio");

$v_strm_out1 = av_stream_open($file_out1, "video", array( "width" => imagesx($canvas), "height" => imagesy($canvas)));
$a_strm_out1 = av_stream_open($file_out1, "audio");

$v_strm_out2 = av_stream_open($file_out2, "video", array( "width" => imagesx($canvas), "height" => imagesy($canvas)));
$a_strm_out2 = av_stream_open($file_out2, "audio");

$filter = new ReflectionFilter;
$filter->bottom = 0.70;
$canvas_width = imagesx($canvas);
$canvas_height = imagesy($canvas);
$background_color = imagecolorallocate($canvas, 0, 0, 0);

set_time_limit(600);

$v_time = 0;
$a_time = 0;
while(!av_file_eof($file_in)) {
	if($v_time < $a_time || !isset($a_strm_in)) {
		if(av_stream_read_image($v_strm_in, $image, $v_time)) {
			imagefilledrectangle($canvas, 0, 0, $canvas_width, $canvas_height, $background_color);
			$filter->draw($canvas, $image);
			av_stream_write_image($v_strm_out1, $canvas, $v_time);
			av_stream_write_image($v_strm_out2, $canvas, $v_time);
		} else {
			$v_time = INF;
		}
	} else {
		if(av_stream_read_pcm($a_strm_in, $pcm, $a_time)) {
			av_stream_write_pcm($a_strm_out1, $pcm, $a_time);
			av_stream_write_pcm($a_strm_out2, $pcm, $a_time);
		} else {
			$a_time = INF;
		}
	}
}

$end = microtime(true);
$duration = $end - $start;
echo "$duration seconds\n";

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
	
	/** @var bool */
	protected static $compiled;
	
	public function __construct($bottom = 0.75, $gap = 0.02, $scale = 0.6, $reflectivity = 0.5, $length = 0.5) {
		$this->bottom = $bottom;
		$this->gap = $gap;
		$this->scale = $scale;
		$this->reflectivity = $reflectivity;
		$this->length = $length;
		if(!self::$compiled) {
			qb_compile();
			self::$compiled = true;
		}
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
		
		for($dest_y = $dest_top, $refl_y = $refl_bottom, $image_y = 0; $dest_y < $dest_bottom; $dest_y++, $refl_y--, $image_y += $image_adv) {
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
}

?>