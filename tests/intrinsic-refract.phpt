--TEST--
Refraction test
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
	$b = array(1, 2, 3);
	$c = array(10, 5);
	$d = array(-1, -1);
	$e = array(array(1, 1), array(2, 3), array(0, -4), array(5, 0));
	$f = array(1, -1);
	
	echo refract($a, $b, 0.5), "\n";
	echo refract($c, $d, 0.2), "\n";
	echo refract($e, $f, 0.9), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[-2.982051, -6.214102, -9.446153]
[2.1559467676119, 1.1559467676119]
[0.46411010564593, 1.3358898943541, 1.7, 2.8, -7.2262928728937, 3.6262928728937, -4.5210618221829, 9.0210618221829]
