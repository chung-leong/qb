--TEST--
Function call with default value test (scalar)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[]		$a
 * @param	int32		$b
 */
function other_function($a, $b = 1234) {
	echo "$a $b";
}

/**
 * @engine	qb
 * @local	int32[4]	$a
 */
function test_function() {
	$a = array(1, 2, 3, 4);
	other_function($a);
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4] 1234
