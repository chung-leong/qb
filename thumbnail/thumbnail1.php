<?php

$folder = dirname(__FILE__);
$path = "$folder/The Princess Bride.mp4";
$file = av_file_open($path, "r");
$stat = av_file_stat($file);
$strm = av_stream_open($file, "video");

$video_stat = $stat["streams"]["video"];
$width = $video_stat["width"];
$height = $video_stat["height"];

$thumb_width = 320;
$thumb_height = $thumb_width * ($height / $width);
$image = imagecreatetruecolor($thumb_width, $thumb_height);
av_file_seek($file, 1 * 60, false);
av_stream_read_image($strm, $image, $time);
imagejpeg($image, "thumb.jpg");

?>