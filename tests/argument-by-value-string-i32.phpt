--TEST--
Argument by value test (byte string, I32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Numbers: $a";
}

$data = pack('i*', 165535, 365534, -1, 0, 444444444);

test_function($data);

?>
--EXPECT--
Numbers: [165535, 365534, -1, 0, 444444444]