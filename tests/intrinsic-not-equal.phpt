--TEST--
Vector not-equal test
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
	$a = array(1, 2, 3);
	$b = array(1, 2, 3);
	$c = array(1, 2, 4);
	$d = 1;
	$e = 2;
	$f = 2;
	$k = array(1, 2, 3);
	echo not_equal($a, $b), "\n";
	echo not_equal($a, $c), "\n";
	echo not_equal($a, $e), "\n";
	echo not_equal($d, $e), "\n";
	echo not_equal($e, $f), "\n";
	echo not_equal($a, $k), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0, 0, 0]
[0, 0, 1]
[1, 0, 1]
1
0
[0, 0, 0]
