<?php

// the number of seconds to pause at each image
define('PAUSE_TIME', 5);

// output frame-rate
define('FRAME_RATE', 12.0);
define('FRAME_DURATION', 1.0 / FRAME_RATE);

$folder = dirname(__FILE__);
$filenames = array("malgorzata_socha_1.jpg", "malgorzata_socha_2.jpg", "malgorzata_socha_3.jpg", "malgorzata_socha_4.jpg", "malgorzata_socha_5.jpg");
$width = 640;
$height = 480;

// add the first filename to the end of the list so the video loops correctly
$filenames[] = $filenames[0];

// open the video files containing the transition effect
$tran_in_file = av_file_open("$folder/card-wipe-in-rtl.flv", "r");
$tran_out_file = av_file_open("$folder/card-wipe-out-rtl.flv", "r");

// open video streams
$tran_in_strm = av_stream_open($tran_in_file, "video");
$tran_out_strm = av_stream_open($tran_out_file, "video");

// create buffers for images in transition
$tran_in_img = imagecreatetruecolor($width, $height);
$tran_out_img = imagecreatetruecolor($width, $height);

// the combined image
$tran_final_img = imagecreatetruecolor($width, $height);
imagealphablending($tran_final_img, true);
$bk_color = imagecolorallocate($tran_final_img, 255, 255, 255);

// open output video files 
$output1_file = av_file_open("$folder/output.mp4", "w");
$output2_file = av_file_open("$folder/output.webm", "w");

// open output video streams
$output1_strm = av_stream_open($output1_file, "video", array( 'width' => $width, 'height' => $height, 'bit_rate' => 200 * 1000 ));
$output2_strm = av_stream_open($output2_file, "video", array( 'width' => $width, 'height' => $height, 'bit_rate' => 200 * 1000 ));

// load first image
$index = 0;
$current_name = $filenames[$index];
$current_img = imagecreatefromjpeg("$folder/$current_name");
$next_img = null;

/**
 * Multiply an image by a mask
 * 
 * @engine qb
 * @param image $.*
 */
function apply_mask(&$dst, $image, $mask) {
	$dst = $image * $mask;
}

qb_compile();

set_time_limit(60);
$start_time = microtime(true);

// loop through the list of filenames
$time = 0.0;
$pause_time = PAUSE_TIME;
$done = false;
while(!$done) {
	if($pause_time > 0) {
		// still waiting at the current image
		av_stream_write_image($output1_strm, $current_img, $time);
		av_stream_write_image($output2_strm, $current_img, $time);
		$time += FRAME_DURATION;
		$pause_time -= FRAME_DURATION;
	} else {
		// transition to next image
		if(!$next_img) {
			$next_name = $filenames[$index + 1];
			$next_img = imagecreatefromjpeg("$folder/$next_name");
			
			// seek back to beginning of videos
			av_file_seek($tran_in_file, 0);
			av_file_seek($tran_out_file, 0);
		}
		
		// read in the image masks
		if(av_stream_read_image($tran_in_strm, $tran_in_img, $tran_in_time) && av_stream_read_image($tran_out_strm, $tran_out_img, $tran_out_time)) {
			// apply the transition masks
			apply_mask($tran_out_img, $current_img, $tran_out_img);
			apply_mask($tran_in_img, $next_img, $tran_in_img);
			
			// put one on top of the other
			imagefilledrectangle($tran_final_img, 0, 0, $width, $height, $bk_color);
			imagecopy($tran_final_img, $tran_in_img, 0, 0, 0, 0, $width, $height);
			imagecopy($tran_final_img, $tran_out_img, 0, 0, 0, 0, $width, $height);
			
			// encode the result
			av_stream_write_image($output1_strm, $tran_final_img, $time);
			av_stream_write_image($output2_strm, $tran_final_img, $time);
			$time += FRAME_DURATION;
		} else {
			$index++;
			if($index + 1 < count($filenames)) {
				// the next image becomes the current image
				$current_img = $next_img;
				$current_name = $next_name;
				$next_img = null;
				$pause_time = PAUSE_TIME;
			} else {
				$done = true;
			}
		}
	}
}

$end_time = microtime(true);
$duration = $end_time - $start_time;
echo "$duration seconds";

?>