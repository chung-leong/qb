--TEST--
Complex number inverse tangent test
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
	$a = array(-0.00050798, 1.0004);
	$c[0] = array(0, 0);
	$c[1] = array(0, NAN);
	$c[2] = array(3.5231, +INF);
	
	$c = catan($c);
	echo catan($a), "\n";
	echo "$c\n";
	echo catan(array(0.4, 0.8)), "\n";
	echo catan(array(4.5, NAN)), "\n";
	echo catan(array(-INF, 8)), "\n";
	echo catan(array(+INF, 12)), "\n";
	echo catan(array(-INF, +INF)), "\n";
	echo catan(array(+INF, +INF)), "\n";
	echo catan(array(+INF, NAN)), "\n";
	echo catan(array(NAN, 0.5)), "\n";
	echo catan(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-1.119, 4.0186]
[[0, 0], [NAN, NAN], [1.5708, 0]]
[0.6629088318, 0.708303336]
[NAN, NAN]
[-1.570796327, 0]
[1.570796327, 0]
[-1.570796327, 0]
[1.570796327, 0]
[1.570796327, 0]
[NAN, NAN]
[NAN, 0]

