<?php

trait SequentialNumbers {

	function arguments($count) {
		$arg1 = range(0, $count);
		return array($arg1);
	}
}

require("Arithmetic.php");
require("Math.php");

function check_if_faster($obj, $count, $iterations = 100) {
	$arguments = $obj->arguments($count);

	$ratios = array();
	$func = array($obj, 'test');
	for($i = 0; $i < $iterations; $i++) {
		ini_set("qb.multithreading_threshold", 1);
		$mt_time = call_user_func_array($func, $arguments);

		ini_set("qb.multithreading_threshold", $count * 2);
		$nmt_time = call_user_func_array($func, $arguments);

		$ratios[] = $mt_time / $nmt_time;
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

find_optimal('Sqrt_F64');

?>
