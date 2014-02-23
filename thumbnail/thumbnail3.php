<?php

/**
  * @engine qb
  * @param float32[][4]			$image
  *
  * @local float32[4][4]		$YCbCrMatrix
  * @local float32[r,g,b,a]		$pixel
  * @local float32[y,cb,cr,a]	$ycbcr
  * @local uint32				$index
  * @local float32				$count
  *
  * @return float32
  */
function get_skin_color_pixel_ratio($image) {
	$YCbCrMatrix = array(
		array(	0.299,	-0.169,	0.500,	0.000	),
		array(	0.587,	-0.331,	-0.419,	0.000	),
		array(	0.114,	0.500,	-0.081,	0.000	),
		array(	0.000,	0.500,	0.500,	1.000	),
	);
	foreach($image as $index => $pixel) {
		$ycbcr = mv_mult($YCbCrMatrix, $pixel);
		if($ycbcr->cb > 0.412 && $ycbcr->cb < 0.530 && $ycbcr->cr > 0.550 && $ycbcr->cr < 0.647) {
			$count++;
		}
	}
	return $count / count($image);
}

qb_compile();

$folder = dirname(__FILE__);

$path = "$folder/The Princess Bride.mp4";
$path = "$folder/Blade Runner.mkv";
//$path = "$folder/Das Leben der Anderen.mp4";
//$path = "$folder/Donnie Darko.mp4";
//$path = "$folder/Shadowlands.avi";
//$path = "$folder/Och Karol 2.avi";
//$path = "$folder/Crouching Tiger Hidden Dragon.avi";
//$path = "$folder/Run Lola Run.avi";
//$path = "$folder/Goodbye Lenin.avi";
//$path = "$folder/U Pana Boga za piecem.avi";
//$path = "$folder/The Celebration.avi";
$file = av_file_open($path, "r");
$strm = av_stream_open($file, "video");
$stat = av_file_stat($file);

$video_stat = $stat["streams"]["video"];
$width = $video_stat["width"];
$height = $video_stat["height"];
$duration = $stat["duration"];

$thumb_width = 320;
$thumb_height = $thumb_width * ($height / $width);
$thumb_path = "$path.jpg";

$current_deviation = INF;
$current_image = null;
for($i = 1; $i < 20 && $i < $duration; $i += 0.5) {
	$image = imagecreatetruecolor($thumb_width, $thumb_height);
	av_file_seek($file, $i * 60, false);
	av_stream_read_image($strm, $image, $time);
	$ratio = get_skin_color_pixel_ratio($image);
	$deviation = abs($ratio - 0.25);
	if($ratio < $current_deviation) {
		$current_image = $image;
		$current_deviation = $deviation;
	}
}

imagejpeg($current_image, $thumb_path);

?>