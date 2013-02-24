--TEST--
While loop test
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
function test_function($n) {
	$k = 0;
	while($n < 100) {
		$k += 10;
		$n++;
	}
	return $k;
}

qb_compile();

echo test_function(99), "\n";
echo test_function(5), "\n";

?>
--EXPECTREGEX--
10
950
