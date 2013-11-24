--TEST--
Yield test
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
	for($n = 0; $n < 10; $n++) {
		$k += 10;
		yield $k;
	}
}

foreach(test_function() as $num) {
	echo "$num\n";
}


?>
--EXPECT--
11
21
31
41
51
61
71
81
91
101
