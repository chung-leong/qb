--TEST--
Complex number exponential test
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
	$a = $b = array(2, 4);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = cexp($c);
	echo cexp($a), "\n";
	echo exp($b), "\n";
	echo "$c\n";
	echo cexp(array(0, 0)), "\n";
	echo cexp(array(2.5, +INF)), "\n";
	echo cexp(array(4.5, NAN)), "\n";
	echo cexp(array(+INF, 0)), "\n";
	echo cexp(array(-INF, 1)), "\n";
	echo cexp(array(+INF, 1)), "\n";
	echo cexp(array(-INF, +INF)), "\n";
	echo cexp(array(+INF, +INF)), "\n";
	echo cexp(array(-INF, NAN)), "\n";
	echo cexp(array(+INF, NAN)), "\n";
	echo cexp(array(NAN, 0.0)), "\n";
	echo cexp(array(NAN, 0.5)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-4.8298, -5.5921]
[-4.8298, -5.5921]
[[-18.301, -8.2776], [1.2213E-6, 1.902E-6], [-13.781, -30.112]]
[1, 0]
[NAN, NAN]
[NAN, NAN]
[INF, 0]
[0, 0]
[INF, INF]
[0, 0]
[INF, NAN]
[0, 0]
[INF, NAN]
[NAN, 0]
[NAN, NAN]

