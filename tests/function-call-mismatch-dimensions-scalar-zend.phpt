--TEST--
Function call array dimension mismatch test (scalar from PHP)
--FILE--
<?php

/**
 * @engine	qb
 * @param	int32[4][8]			$a
 * @param	int32[][3][4]		$b
 */
function other_function($a, $b = array()) {
	echo "$a\n";
	echo "$b\n";
}

function test_function() {
	$a = 8;
	other_function($a);
}

test_function();

?>
--EXPECT--
[[8, 8, 8, 8, 8, 8, 8, 8], [8, 8, 8, 8, 8, 8, 8, 8], [8, 8, 8, 8, 8, 8, 8, 8], [8, 8, 8, 8, 8, 8, 8, 8]]
[]

