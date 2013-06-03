<?php

$folder = dirname(__FILE__);

// open source file
$file_in = av_file_open("$folder/source-code.mov", "r");
$stat_in = av_file_stat($file_in);

// open output files
$file_out1 = av_file_open("$folder/source-code.mp4", "w");
$file_out2 = av_file_open("$folder/source-code.webm", "w");

// create image buffer
$width = $stat_in['streams']['video']['width'];
$height = $stat_in['streams']['video']['height'];
$image = imagecreatetruecolor($width, $height);

// open input streams
$a_strm_in = av_stream_open($file_in, "audio");
$v_strm_in = av_stream_open($file_in, "video");

// open output streams
$a_strm_out1 = av_stream_open($file_out1, "audio");
$v_strm_out1 = av_stream_open($file_out1, "video", array( "width" => $width, "height" => $height ));

$a_strm_out2 = av_stream_open($file_out2, "audio");
$v_strm_out2 = av_stream_open($file_out2, "video", array( "width" => $width, "height" => $height ));

$v_time = 0;
$a_time = 0;
while(!av_file_eof($file_in)) {
  // read from video stream if it's behind the audio stream
  if($v_time < $a_time) {
    // read video frame
    if(av_stream_read_image($v_strm_in, $image, $v_time)) {
      // write video frame
      av_stream_write_image($v_strm_out1, $image, $v_time);
      av_stream_write_image($v_strm_out2, $image, $v_time);
    } else {
      // no more video frames
      $v_time = INF;
    }
  } else {
    // read audio segment
    if(av_stream_read_pcm($a_strm_in, $pcm, $a_time)) {
      // write audio segment
      av_stream_write_pcm($a_strm_out1, $pcm, $a_time);
      av_stream_write_pcm($a_strm_out2, $pcm, $a_time);
    } else {
      // no more audio
      $a_time = INF;
    }
  }
}

// close files
av_stream_close($a_strm_in);
av_stream_close($v_strm_in);
av_file_close($file_in);

av_stream_close($a_strm_out1);
av_stream_close($v_strm_out1);
av_file_close($file_out1);

av_stream_close($a_strm_out2);
av_stream_close($v_strm_out2);
av_file_close($file_out2);

?>