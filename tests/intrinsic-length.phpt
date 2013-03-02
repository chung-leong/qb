--TEST--
Vector length test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]		$a
 * @local	float32[3]		$b
 * @local	float32[2]		$c
 * @local	float64[5]		$d
 * @local	float64[*][4]	$e
 * @local	float64[*]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5, 0.5);
	$b = array(1, 2, 3);
	$c = array(10, 5);
	$d = array(1, 2, 3, 4, 5);
	$e[] = array(1, -1, 1, 1);
	$e[] = array(2, -2, -2, 2);
	$e[] = array(1, 2, 3, 4);
	
	echo length($a), "\n";
	echo length($b), "\n";
	echo length($c), "\n";
	echo length($d), "\n";
	$f = length($e);
	echo $f, "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1
3.741657
11.18034
7.4161984870957
[2, 4, 5.4772255750517]
