--TEST--
Variable interpolation test (array)
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
	$c = 3000;
	$b = array(9, 0, 0xFF);
	echo "Numbers: $a, $b, $c";
}

test_function();

?>
--EXPECT--
Numbers: [67, 1234, 88, -315], [9, 0, -1], 3000
