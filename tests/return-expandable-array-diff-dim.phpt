--TEST--
Return expandable array test (different dimensions)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * 
 * @return	int32[*][4]
 * 
 */
function test_function() {
	$a[] = 1;
	$a[] = 2;
	$a[] = 3;
	$a[] = 4;
	$a[] = 10;
	$a[] = 20;
	$a[] = 30;
	$a[] = 40;
	$a[] = 100;
	$a[] = 200;
	$a[] = 300;
	$a[] = 400;
	return $a * 2;
}

print_r(test_function());

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => 2
            [1] => 4
            [2] => 6
            [3] => 8
        )

    [1] => Array
        (
            [0] => 20
            [1] => 40
            [2] => 60
            [3] => 80
        )

    [2] => Array
        (
            [0] => 200
            [1] => 400
            [2] => 600
            [3] => 800
        )

)
