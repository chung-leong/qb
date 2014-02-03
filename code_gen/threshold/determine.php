<?php

trait SequentialNumbers {

	function arguments($count) {
		$arg1 = range(0, $count);
		return array($arg1);
	}
}

trait SequentialInverse {

	function arguments($count) {
		$arg1 = array();
		for($i = 1; $i <= $count; $i++) {
			$arg1[] = 1 / $i;
		}
		return array($arg1);
	}
}

trait Image {

	function arguments($count) {
		$width = 256;
		$height = $count / $width;
		$arg1 = imagecreatetruecolor($width, $height);
		$color = imagecolorallocate($arg1, 255, 128, 128);
		imagefilledrectangle($arg1, 0, 0, $width, $height, $color);
		$color = imagecolorallocatealpha($arg1, 255, 128, 128, 100);
		imagefilledrectangle($arg1, 0, 0, $width / 2, $height / 2, $color);
		return array($arg1);
	}
}

require("Arithmetic.php");
require("Math.php");
require("Pixel.php");

function check_if_faster($obj, $count, $iterations = 500) {
	$arguments = $obj->arguments($count);

	$ratios = array();
	$func = array($obj, 'test');
	for($i = 0; $i < $iterations; $i++) {
		ini_set("qb.multithreading_threshold", 1);
		$mt_time = call_user_func_array($func, $arguments);

		ini_set("qb.multithreading_threshold", $count * 2);
		$nmt_time = call_user_func_array($func, $arguments);

		if($nmt_time > 0) {
			$ratios[] = $mt_time / $nmt_time;
		} else {
			$ratios[] = INF;
		}
	}
	sort($ratios);
	$median = $ratios[$iterations >> 1];
	echo "$count: $median\n";
	return $median < 0.80;
}

function find_optimal($class) {
	$obj = new $class;
	for($i = 1024; $i < 1024 * 1024; $i *= 2) {
		if(check_if_faster($obj, $i)) {
			for($j = $i / 2 + 1024; $j < $i; $j += 1024) {
				if(check_if_faster($obj, $j)) {
					return $j;
				}
			}
		}
	}
	return 0;
}

find_optimal('Add_F64');

?>
