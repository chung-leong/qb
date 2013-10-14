--TEST--
Argument by value test (byte string, F32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('f*', 3.14, 1000.5, 1/3);

test_function($data);

?>
--EXPECT--
Numbers: [3.14, 1000.5, 0.3333333]