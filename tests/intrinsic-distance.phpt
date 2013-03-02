--TEST--
Distance test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3]		$a
 * @local	float32[3]		$b
 * @local	float32[2]		$c
 * @local	float64[2]		$d
 * @local	float64[][2]	$e
 * @local	float64[2]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5);
	$b = array(1.5, 2.5, 3.5);
	$c = array(10, 5);
	$d = array(10, 5.1);
	$e = array(array(1, 1), array(2, 3), array(0, -4), array(5, 0));
	$f = array(0, 0);
	
	echo distance($a, $b), "\n";
	echo distance($c, $d), "\n";
	echo distance($e, $f), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
3.741657
0.1
[1.4142135623731, 3.605551275464, 4, 5]
