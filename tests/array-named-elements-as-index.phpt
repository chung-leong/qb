--TEST--
Array with named elements as index test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]		$a
 * @local	float32[x,y]	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[$c->x] = 5;
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[5, 0, 0, 0]
