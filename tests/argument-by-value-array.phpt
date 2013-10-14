--TEST--
Argument by value test (array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[4]	$a
 * @param	float64[2]	$b
 * 
 * @return	void
 * 
 */
function test_function($a, $b) {
	echo "$a $b";
}

$b = array(M_PI, M_PI / 2);

test_function(array(333, 444, 555, 666), $b);

?>
--EXPECT--
[333, 444, 555, 666] [3.1415926535898, 1.5707963267949]