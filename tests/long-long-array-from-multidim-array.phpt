--TEST--
64 bit integer array from 2-d array test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int64[]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "$a\n";
}

qb_compile();

$a = array(
	array(0xFFFFFFFFF, 0xFFFFFFFF), 
	array(0x00FFFFFF, 0x00FFFFFF),
	1234,
	array(1, 2),
);

test_function($a);

?>
--EXPECT--
[-1, 72057589759737855, 1234, 4294967298]
