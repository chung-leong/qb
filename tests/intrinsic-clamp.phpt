--TEST--
Clamp test
--SKIPIF--
<?php
	if(!is_nan(NAN)) print "skip borken NAN constant";
?>
--FILE--
<?php

/**
 * @engine qb
 * @param float64[]	$a
 */
function test_function($a) {
	echo clamp($a, 10, 100), "\n";
}

qb_compile();

test_function(array(1, 34, 999, 99, 17, 44, INF, NAN));

?>
--EXPECT--
[10, 34, 100, 99, 17, 44, 100, NAN]
