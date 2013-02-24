--TEST--
Minimum number test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$a
 * @local	int32	$b
 * @local	uint32	$c
 * @local	int8	$d
 * @local	int64	$e
 * @local	float64	$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1;
	$b = -22;
	$c = 3;
	$d = 127;
	$e = -300000;
	$f = 1.5;
	echo min($a, $b), "\n";
	echo min($a, $c), "\n";
	echo min($a, $f), "\n";
	echo min($c, $f), "\n";
	echo min($e, $d), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
-22
1
1
1.5
-300000
