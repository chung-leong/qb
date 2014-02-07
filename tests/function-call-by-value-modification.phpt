--TEST--
Function call by value test  (argument modification)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[]		$a
 */
function other_function($a) {
	$a[2] = 4;
}

/**
 * @engine	qb
 * @local	int32[3]	$a
 */
function test_function() {
	$a = array(1, 2, 3);
	echo $a, "\n";
	other_function($a);
	echo $a, "\n";
}

test_function();

?>
--EXPECT--
[1, 2, 3]
[1, 2, 3]
