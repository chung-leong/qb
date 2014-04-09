--TEST--
Complex number hyperbolic tangent test
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
	
	$c = ctanh($c);
	echo ctanh($a), "\n";
	echo "$c\n";
	echo ctanh(array(0, 0)), "\n";
	echo ctanh(array(2.5, +INF)), "\n";
	echo ctanh(array(4.5, NAN)), "\n";
	echo ctanh(array(+INF, 12)), "\n";
	echo ctanh(array(-INF, +INF)), "\n";
	echo ctanh(array(+INF, +INF)), "\n";
	echo ctanh(array(+INF, NAN)), "\n";
	echo ctanh(array(NAN, 0.0)), "\n";
	echo ctanh(array(NAN, 0.5)), "\n";
	echo ctanh(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[1.0047, 0.036423]
[[0.99673, 0.0037109], [-1, 9.2914E-12], [1.0012, 0.0013819]]
[0, 0]
[NAN, NAN]
[NAN, NAN]
[1, 0]
[-1, 0]
[1, 0]
[1, 0]
[NAN, 0]
[NAN, NAN]
[NAN, NAN]

