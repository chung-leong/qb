--TEST--
Maximum number test
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
	echo max($a, $b), "\n";
	echo max($a, $c), "\n";
	echo max($a, $f), "\n";
	echo max($c, $f), "\n";
	echo max($e, $d), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1
3
1.5
3
127
