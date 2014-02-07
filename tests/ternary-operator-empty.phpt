--TEST--
Ternary operator test (empty true clause)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3.0') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$a
 * @local	int32	$b
 * @local	int32	$c
 * 
 * @return	void
 * 
 */
function test_function($a) {
	$c = -12;
	$b = $a ? : $c + 456;
	echo "$b\n";
}

test_function(123);
test_function(0);

?>
--EXPECT--
123
444
