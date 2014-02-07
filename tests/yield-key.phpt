--TEST--
Yield test (with key)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5.0') < 0) print 'skip feature not available';
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
	for($n = 1; $n < 11; $n++) {
		$k += 10;
		yield $n => $k;
	}
}

foreach(test_function() as $index => $num) {
	echo "$index => $num\n";
}


?>
--EXPECT--
1 => 11
2 => 21
3 => 31
4 => 41
5 => 51
6 => 61
7 => 71
8 => 81
9 => 91
10 => 101
