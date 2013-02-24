--TEST--
All true test
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
	echo all($a), "\n";
	echo all($b), "\n";
	echo all($c), "\n";
	echo all($k), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1
0
0
1
