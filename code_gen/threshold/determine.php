<?php

$predefined_classes = get_declared_classes();

trait SequentialNumbers {

	function arguments($count) {
		$arg1 = range(1, $count);
		return array($arg1);
	}
}

trait SequentialNumbersWithNegative {

	function arguments($count) {
		$start = (int) - $count / 4;
		$end = $start + $count;
		$arg1 = range($start, $end);
		return array($arg1);
	}
}

trait SequentialInverses {

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

trait Sampling {

	function arguments($count) {
		$width = 256;
		$height = 256;
		$arg1 = imagecreatetruecolor($width, $height);
		$color = imagecolorallocate($arg1, 255, 128, 128);
		imagefilledrectangle($arg1, 0, 0, $width, $height, $color);
		$color = imagecolorallocatealpha($arg1, 255, 128, 128, 100);
		imagefilledrectangle($arg1, 0, 0, $width / 2, $height / 2, $color);

		$arg2 = array();
		$arg3 = array();
		$x = 0.5;
		$y = 0.5;
		for($i = 0; $i < $count; $i++) {
			if($x > $width) {
				$x = 0.5;
			}
			if($y > $height) {
				$y = 0.5;
			}
			$arg2[] = $x;
			$arg3[] = $y;
			$x += 2;
			$y += 4;
		}
		return array($arg1, $arg2, $arg3);
	}
}

require("Arithmetic.php");
require("Bitwise.php");
require("ComplexNumber.php");
require("Math.php");
require("Matrix.php");
require("Pixel.php");

function check_if_faster($obj, $count, $iterations = 100) {
	if(property_exists($obj, 'vector_size')) {
		$count -= $count % $obj->vector_size;
	}
	$arguments = $obj->arguments($count);

	$ratios = array();
	$func = array($obj, 'test');
	for($i = 0; $i < $iterations; $i++) {
		ini_set("qb.thread_count", 0);
		$mt_time = call_user_func_array($func, $arguments);

		ini_set("qb.thread_count", 1);
		$nmt_time = call_user_func_array($func, $arguments);

		if($nmt_time > 0) {
			$ratios[] = $mt_time / $nmt_time;
		} else {
			$ratios[] = INF;
		}
	}
	sort($ratios);
	$median = $ratios[$iterations >> 1];
	return $median < 0.80;
}

function find_optimal($class) {
	$obj = new $class;
	for($i = 1024; $i <= 1024 * 1024; $i *= 2) {
		if(check_if_faster($obj, $i)) {
			return $i;
		}
	}
	return 0;
}

$classes = array_diff(get_declared_classes(), $predefined_classes);

$f = fopen("multithreading_thresholds.txt", "wb");

foreach($classes as $class) {
	if(preg_match('/(\w+)_(\w\d{2})_X(\d)/', $class, $m) || preg_match('/(\w+)_(\w\d{2})/', $class, $m)) {
		$name = $m[1];
		$type = $m[2];
		$width = isset($m[3]) ? (int) $m[3] : 1;
		$threshold = find_optimal($class);
		fwrite($f, "$name	$type	$width" . PHP_EOL);
	} else {
		echo "$class\n";
		die();
	}
}
	

?>
