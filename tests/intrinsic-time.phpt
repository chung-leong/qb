--TEST--
Time test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64	$b
 * 
 * @return	float64
 * 
 */
function test_function() {
	$b = time();
	return $b;
}

$a = microtime(true);
$b = test_function();

printf("%f\n", $b - $a);

?>
--EXPECTREGEX--
0\.\d+
