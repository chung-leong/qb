--TEST--
Echo variable test (scalar, u64)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.2') < 0) print 'skip broken 64 bit output';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint64	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0x000FFFFFFFFFFFF7; 
	echo $a;
}

test_function();

?>
--EXPECT--
4503599627370487
