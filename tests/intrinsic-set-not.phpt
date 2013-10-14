--TEST--
Set inversion test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[3]	$[abc]
 * @local	int64[3]	$k
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 1, 1);
	$b = array(1, 0, 1);
	$c = array(0, 0, 0);
	$k = array(100, 200, 301);
	echo not($a), "\n";
	echo not($b), "\n";
	echo not($c), "\n";
	echo not($k), "\n";
}

test_function();

?>
--EXPECT--
[0, 0, 0]
[0, 1, 0]
[1, 1, 1]
[0, 0, 0]
