--TEST--
Complex component by name test
--FILE--
<?php

/**
 * @engine qb
 * @local complex<float64> $a
 */
function test_function() {
	$a->r = 3;
	$a->i = 2;
	echo "$a\n";
	echo "$a->r, $a->i\n";
}

test_function();

?>
--EXPECT--
[3, 2]
3, 2
