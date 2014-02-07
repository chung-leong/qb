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

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-2.9821, -6.2141, -9.4462]
[2.155946768, 1.155946768]
[[0.4641101056, 1.335889894], [1.7, 2.8], [-7.226292873, 3.626292873], [-4.521061822, 9.021061822]]
