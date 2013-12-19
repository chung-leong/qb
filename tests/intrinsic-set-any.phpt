--TEST--
Any true test
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
	echo any($a), "\n";
	echo any($b), "\n";
	echo any($c), "\n";
	echo any($k), "\n";
}

test_function();

?>
--EXPECT--
1
1
0
1
