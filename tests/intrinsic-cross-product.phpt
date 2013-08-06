--TEST--
Cross product test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3]		$a
 * @local	float32[3]		$b
 * @local	float32[3]		$c
 * @local	float64[3]		$d
 * @local	float64[][3]	$e
 * @local	float64[3]		$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5);
	$b = array(1, 2, 3);
	$c = array(10, 5, 0);
	$d = array(-1, -1, -1);
	$e = array(array(1, 1, 1), array(1, 2, 3), array(0, 2, -4), array(8, 5, 0));
	$f = array(1, -1, 0.5);
	
	echo cross($a, $b), "\n";
	echo cross($c, $d), "\n";
	echo cross($e, $f), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0.5, -1, 0.5]
[-5, 10, -5]
[[1.5, 0.5, -2], [4, 2.5, -3], [-3, -4, -2], [2.5, -4, -13]]
