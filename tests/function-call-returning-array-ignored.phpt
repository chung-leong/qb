--TEST--
Function call test (ignoring array returned)
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
	other_function();
	echo "$a\n";
}

test_function();

?>
--EXPECT--
0