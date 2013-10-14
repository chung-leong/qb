--TEST--
If statement test
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
	if($n < 10) {
		$n = $n * $n;
	} else {
		$n = 10;
	}
	return $n;
}

echo test_function(5), "\n";
echo test_function(15), "\n";

?>
--EXPECT--
25
10
