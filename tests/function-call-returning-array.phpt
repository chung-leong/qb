--TEST--
Function call test (array return value)
--FILE--
<?php

/**
 * @engine	qb
 * @local	int32[4]	$a
 * @return	int32[4]
 * 
 */
function other_function() {
	$a = array(1, 2, 3, 4);
	return $a;
}

/**
 * @engine	qb
 * @local int32[10]		$a
 */
function test_function() {
	$a = other_function();
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2]
