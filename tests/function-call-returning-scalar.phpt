--TEST--
Function call test (scalar return value)
--FILE--
<?php 

/**
 * @engine qb
 * @local int32		$a
 */
function test_function() {
	$a = other_function();
	echo "$a\n";
}

/**
 * @engine qb
 * @return int32
 */
function other_function() {
	return 1234;
}

qb_compile();

test_function();

?>
--EXPECT--
1234
