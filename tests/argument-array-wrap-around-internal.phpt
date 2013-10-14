--TEST--
Argument wrap-around test (internal)
--FILE--
<?php

/**
 * @engine qb
 * @param float64[12]	$a
 */
function other_function($a) {
	echo "$a\n";
}

/**
 * @engine qb
 * @local uint32[4]		$b
 */
function test_function() {
	$b = array(1, 2, 3, 4);
	other_function($b);
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4]
