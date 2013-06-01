<?php

$start = microtime(true);
$folder = dirname(__FILE__);
$image = imagecreatetruecolor(852, 480);
$file_in = av_file_open("$folder/source-code.mov", "r");
$file_out1 = av_file_open("$folder/source-code-sepia-gd.webm", "w");
$file_out2 = av_file_open("$folder/source-code-sepia-gd.mp4", "w");

$v_strm_in = av_stream_open($file_in, "video");
$a_strm_in = av_stream_open($file_in, "audio");

$v_strm_out1 = av_stream_open($file_out1, "video", array( "width" => imagesx($image), "height" => imagesy($image)));
$a_strm_out1 = av_stream_open($file_out1, "audio");

$v_strm_out2 = av_stream_open($file_out2, "video", array( "width" => imagesx($image), "height" => imagesy($image)));
$a_strm_out2 = av_stream_open($file_out2, "audio");

set_time_limit(600);

$v_time = 0;
$a_time = 0;
while(!av_file_eof($file_in)) {
	if($v_time < $a_time || !isset($a_strm_in)) {
		if(av_stream_read_image($v_strm_in, $image, $v_time)) {
			imagefilter($image, IMG_FILTER_GRAYSCALE);
			imagefilter($image, IMG_FILTER_COLORIZE, 50, 25, 0);
			av_stream_write_image($v_strm_out1, $image, $v_time);
			av_stream_write_image($v_strm_out2, $image, $v_time);
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

?>