--TEST--
Argument by value test (byte string, F64)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

qb_compile();

$data = pack('d*', M_PI, 1.5e20, 1/3);

test_function($data);

?>
--EXPECT--
Numbers: [3.1415926535898, 1.5E+20, 0.33333333333333]