--TEST--
Time test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64	$a
 * @local	float64	$b
 * 
 * @return	void
 * 
 */
function test_function($a) {
	$b = time() - $a;
	echo "$b\n";
}

$a = microtime(true);
test_function($a);

$a = microtime(true);
test_function($a);

?>
--EXPECTREGEX--
0\.\d+
