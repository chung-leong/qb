--TEST--
Array with named elements appending test, with explicit class name
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$i
 * @local	float32[x,y]			$b
 * @param	float32[*][Point:x,y]	$a
 * 
 * @return	void
 * 
 */
function test_function(&$a) {
	for($i = 0; $i < 10; $i++) {
		$b->x = $i * 100;
		$b->y = $i * 50;
		$a[] = $b;
	}
}

class Point {
	public $x;
	public $y;
}

$points = array();

test_function($points);

print_r($points);

?>
--EXPECT--
Array
(
    [0] => Point Object
        (
            [x] => 0
            [y] => 0
        )

    [1] => Point Object
        (
            [x] => 100
            [y] => 50
        )

    [2] => Point Object
        (
            [x] => 200
            [y] => 100
        )

    [3] => Point Object
        (
            [x] => 300
            [y] => 150
        )

    [4] => Point Object
        (
            [x] => 400
            [y] => 200
        )

    [5] => Point Object
        (
            [x] => 500
            [y] => 250
        )

    [6] => Point Object
        (
            [x] => 600
            [y] => 300
        )

    [7] => Point Object
        (
            [x] => 700
            [y] => 350
        )

    [8] => Point Object
        (
            [x] => 800
            [y] => 400
        )

    [9] => Point Object
        (
            [x] => 900
            [y] => 450
        )

)
