--TEST--
64 bit integer array from 2-d array test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.2.0') < 0) print 'skip broken 64 bit output';
?>
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
