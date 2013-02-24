--TEST--
64 bit integer from array test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int64	$a
 * @param	uint64	$b
 * 
 * @return	void
 * 
 */
function test_function($a, $b) {
	echo "$a $b\n";
}

qb_compile();

test_function(array(0xFFFFFFFFF, 0xFFFFFFFF), array(0x00FFFFFF, 0x00FFFFFF));

?>
--EXPECT--
-1 72057589759737855