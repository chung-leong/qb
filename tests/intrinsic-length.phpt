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

ini_set('precision', 10);
test_function();

?>
--EXPECT--
1
3.7417
11.18
7.416198487
[2, 4, 5.477225575]
