--TEST--
Function call test (saving array to scalar)
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
 * @local int32		$a
 */
function test_function() {
	$a = other_function();
	echo $a;
}

test_function();

?>
--EXPECTREGEX--
Fatal error.*