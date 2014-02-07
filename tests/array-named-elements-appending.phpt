--TEST--
Array with named elements appending test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$i
 * @local	float32[x,y]	$b
 * @param	float32[*][x,y]	$a
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

$point = new Point;
$point->x = 300;
$point->y = 400;

$points = array($point);

test_function($points);

print_r($points);

?>
--EXPECT--
Array
(
    [0] => Point Object
        (
            [x] => 300
            [y] => 400
        )

    [1] => Point Object
        (
            [x] => 0
            [y] => 0
        )

    [2] => Point Object
        (
            [x] => 100
            [y] => 50
        )

    [3] => Point Object
        (
            [x] => 200
            [y] => 100
        )

    [4] => Point Object
        (
            [x] => 300
            [y] => 150
        )

    [5] => Point Object
        (
            [x] => 400
            [y] => 200
        )

    [6] => Point Object
        (
            [x] => 500
            [y] => 250
        )

    [7] => Point Object
        (
            [x] => 600
            [y] => 300
        )

    [8] => Point Object
        (
            [x] => 700
            [y] => 350
        )

    [9] => Point Object
        (
            [x] => 800
            [y] => 400
        )

    [10] => Point Object
        (
            [x] => 900
            [y] => 450
        )

)
