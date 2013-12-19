--TEST--
Return expandable array test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * 
 * @return	int32[]
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

echo implode(test_function(), ', ');

?>
--EXPECT--
2, 4, 6, 8, 20, 40, 60, 80, 200, 400, 600, 800
