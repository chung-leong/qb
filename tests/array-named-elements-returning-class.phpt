--TEST--
Array with named elements returning object test, explicit class
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[Point:x, y]	$a
 * 
 * @return	float32[Point:x, y]
 * 
 */
function test_function() {
	$a->x = 123;
	$a->y = 456;
	return $a;
}

qb_compile();

class Point {
	public $x;
	public $y;
}

$point = test_function();

print_r($point);

?>
--EXPECT--
Point Object
(
    [x] => 123
    [y] => 456
)