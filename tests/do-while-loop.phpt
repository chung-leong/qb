--TEST--
Do while loop test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$.*
 * 
 * @return	uint32
 * 
 */
function test_function() {
	$a = 0; $b = 0;
	do {
		echo "$a, $b\n";
		$a++;
		$b++;
	} while ($a < 5 || $b < 17);
	return $a;
}

qb_compile();

echo test_function(), "\n";

?>
--EXPECT--
0, 0
1, 1
2, 2
3, 3
4, 4
5, 5
6, 6
7, 7
8, 8
9, 9
10, 10
11, 11
12, 12
13, 13
14, 14
15, 15
16, 16
17
