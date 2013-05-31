<?php

$start = microtime(true);
$folder = dirname(__FILE__);
$image = imagecreatetruecolor(852, 480);
$canvas = imagecreatetruecolor(852, 480);
$file_in = av_file_open("$folder/source-code.mov", "r");
$file_out1 = av_file_open("$folder/source-code-ascii.webm", "w");
$file_out2 = av_file_open("$folder/source-code-ascii.mp4", "w");

$v_strm_in = av_stream_open($file_in, "video");
$a_strm_in = av_stream_open($file_in, "audio");

$v_strm_out1 = av_stream_open($file_out1, "video", array( "width" => imagesx($canvas), "height" => imagesy($canvas), "bit_rate" => 400000 ));
$a_strm_out1 = av_stream_open($file_out1, "audio");

$v_strm_out2 = av_stream_open($file_out2, "video", array( "width" => imagesx($canvas), "height" => imagesy($canvas), "bit_rate" => 400000 ));
$a_strm_out2 = av_stream_open($file_out2, "audio");

$filter_name = "asciimii";
$text = imagecreatefrompng("$folder/fontmap.png");

/**
 * @engine qb
 * @import asciimii.pbj
 *
 * @param image			$dst
 * @param image			$src
 * @param image			$text
 * @param int32			$size
 * @param int32			$charCount
 */
function filter(&$dst, $src, $text, $size, $charCount) {
}

qb_compile();

set_time_limit(600);

$v_time = 0;
$a_time = 0;
while(!av_file_eof($file_in)) {
	if($v_time < $a_time || !isset($a_strm_in)) {
		if(av_stream_read_image($v_strm_in, $image, $v_time)) {
			filter($canvas, $image, $text, 8, 58);
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

?>