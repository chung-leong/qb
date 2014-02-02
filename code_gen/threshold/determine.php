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

require("Arithmetic.php");
require("Math.php");

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
	for($count = 1024; $count < 1024 * 1024; $count += 1024) {
		if(check_if_faster($obj, $count)) {
			return $count;
		}
	}
	return 0;
}

find_optimal('Sin_F64');

?>
