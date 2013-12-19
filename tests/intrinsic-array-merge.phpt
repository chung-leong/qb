--TEST--
Array merge test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[3]	$a
 * @local	int32[5]	$b
 * @local	int64[2]	$c
 * @local	int32[2][2]	$d
 * @local	int32[4][2]	$e
 * @local	int32[]	$r1
 * @local	int32[8]	$r2
 * @local	int32[9]	$r3
 * @local	float64[8]	$r4
 * @local	int32[*][2]	$r5
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3);
	$b = array(4, 5, 6, 7, 8);
	$c = array(-1, -2);
	
	$r1 = array_merge($a, $b);
	/*
	$r2 = array_merge($a, $b);
	$r3 = array_merge($a, $b);
	$r4 = array_merge($a, $b);
	echo "$r1\n";
	echo "$r2\n";
	echo "$r3\n";
	echo "$r4\n";
	echo array_merge($a, $b), "\n";
	$r1 = array_merge($a, $b, $c);
	echo "$r1\n";
	
	echo array_merge($a, $b, -1, -2, -3, $a, array(4, 5)), "\n";
	$d = array(array(1, 2), array(3, 4));
	$e = array(array(5, 6), array(7, 8), array(9, 10), array(11, 12));
	$r5 = array_merge($d, $e);
	echo "$r5\n";
	//echo array_merge($d, $e), "\n";	
	$r5 = array_merge($r5, array(array(13, 14)));
	echo "$r5\n";
	*/
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5, 6, 7, 8]
[1, 2, 3, 4, 5, 6, 7, 8]
[1, 2, 3, 4, 5, 6, 7, 8, 1]
[1, 2, 3, 4, 5, 6, 7, 8]
[1, 2, 3, 4, 5, 6, 7, 8]
[1, 2, 3, 4, 5, 6, 7, 8, -1, -2]
[1, 2, 3, 4, 5, 6, 7, 8, -1, -2, -3, 1, 2, 3, 4, 5]
[[1, 2], [3, 4], [5, 6], [7, 8], [9, 10], [11, 12]]
[[1, 2], [3, 4], [5, 6], [7, 8], [9, 10], [11, 12]]
[[1, 2], [3, 4], [5, 6], [7, 8], [9, 10], [11, 12], [13, 14]]
