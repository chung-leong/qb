--TEST--
Clamp test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.2.0') < 0) print 'skip broken NAN output';
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

test_function(array(1, 34, 999, 99, 17, 44, INF, NAN));

?>
--EXPECT--
[10, 34, 100, 99, 17, 44, 100, NAN]
