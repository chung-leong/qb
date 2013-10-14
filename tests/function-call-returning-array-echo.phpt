--TEST--
Function call test (echo array returned)
--FILE--
<?php 

/**
 * @engine qb
 * @local int32[4]		$a
 * @return int32[4]
 */
function other_function() {
	$a = array(1, 2, 3, 4);
	return $a;
}

/**
 * @engine qb
 */
function test_function() {
	echo other_function();
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4]