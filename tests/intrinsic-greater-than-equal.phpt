--TEST--
Vector greater-than-equal test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[3]	$[abc]
 * @local	int32		$[def]
 * @local	int64[3]	$k
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(100, 200, 300);
	$b = array(101, 102, 103);
	$c = array(100, 201, 300);
	$d = 100;
	$e = 200;
	$f = 200;
	$k = array(100, 200, 301);
	echo greater_than_equal($a, $b), "\n";
	echo greater_than_equal($a, $c), "\n";
	echo greater_than_equal($a, $e), "\n";
	echo greater_than_equal($d, $e), "\n";
	echo greater_than_equal($e, $f), "\n";
	echo greater_than_equal($a, $k), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0, 1, 1]
[1, 0, 1]
[0, 1, 1]
0
1
[1, 1, 0]
