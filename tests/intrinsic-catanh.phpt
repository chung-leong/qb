--TEST--
Complex number inverse hyperbolic tangent test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]			$a
 * @local	complex<float32>	$b
 * @local	float32[3][2]		$c
 *
 * @return	void
 *
 */
function test_function() {
	$a = $b = array(1.0047, 0.036423);
	$c[0] = array(0, 0);
	$c[1] = array(0, NAN);
	$c[2] = array(3.5231, +INF);
	
	$c = catanh($c);
	echo catanh($a), "\n";
	echo atanh($b), "\n";
	echo "$c\n";
	echo catanh(array(0.4, 0.8)), "\n";
	echo catanh(array(4.5, NAN)), "\n";
	echo catanh(array(-INF, 8)), "\n";
	echo catanh(array(+INF, 12)), "\n";
	echo catanh(array(-INF, +INF)), "\n";
	echo catanh(array(+INF, +INF)), "\n";
	echo catanh(array(+INF, NAN)), "\n";
	echo catanh(array(NAN, 0.5)), "\n";
	echo catanh(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[2, 0.85865]
[2, 0.85865]
[[0, 0], [0, NAN], [0, 1.5708]]
[0.2388778613, 0.7232206661]
[NAN, NAN]
[-0, 1.570796327]
[0, 1.570796327]
[-0, 1.570796327]
[0, 1.570796327]
[0, NAN]
[NAN, NAN]
[0, 1.570796327]

