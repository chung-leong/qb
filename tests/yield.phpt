--TEST--
Yield test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$.*
 * @param	uint32	$.*
 * @return	uint32
 * 
 */
function test_function() {
	$k = 1;
	for($n = 0; $n < 10; $n++) {
		$k += 10;
		yield $k;
	}
}

foreach(test_function() as $index => $num) {
	echo "$index => $num\n";
}
foreach(test_function() as $index => $num) {
	echo "$index => $num\n";
}


?>
--EXPECT--
0 => 11
1 => 21
2 => 31
3 => 41
4 => 51
5 => 61
6 => 71
7 => 81
8 => 91
9 => 101
0 => 11
1 => 21
2 => 31
3 => 41
4 => 51
5 => 61
6 => 71
7 => 81
8 => 91
9 => 101
