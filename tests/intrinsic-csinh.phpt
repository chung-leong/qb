--TEST--
Complex number hyperbolic sine test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]		$a
 * @local	float32[3][2]	$c
 *
 * @return	void
 *
 */
function test_function() {
	$a = array(2, 4);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = csinh($c);
	echo csinh($a), "\n";
	echo "$c\n";
	echo csinh(array(0, 0)), "\n";
	echo csinh(array(0, +INF)), "\n";
	echo csinh(array(0, NAN)), "\n";
	echo csinh(array(0.5, +INF)), "\n";
	echo csinh(array(4.5, NAN)), "\n";
	echo csinh(array(+INF, 0)), "\n";
	echo csinh(array(+INF, 12)), "\n";
	echo csinh(array(+INF, +INF)), "\n";
	echo csinh(array(+INF, NAN)), "\n";
	echo csinh(array(NAN, 0)), "\n";
	echo csinh(array(NAN, 0.5)), "\n";
	echo csinh(array(NAN, NAN)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-2.3707, -2.8472]
[[-9.1276, -4.1491], [-1.1952E+5, 1.8614E+5], [-6.8842, -15.07]]
[0, 0]
[0, NAN]
[0, NAN]
[NAN, NAN]
[NAN, NAN]
[INF, 0]
[INF, -INF]
[INF, NAN]
[INF, NAN]
[NAN, 0]
[NAN, NAN]
[NAN, NAN]

