--TEST--
Variable interpolation test (array element)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int16[4]	$a
 * @local	int8[3]		$b
 * @local	uint32		$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(67, 1234, 88, -315);
	$b = array(9, 0, 0xff);
	$c = 3000;
	echo "Numbers: {$a[1]}, {$b[2]}, $c";
}

test_function();

?>
--EXPECT--
Numbers: 1234, -1, 3000
