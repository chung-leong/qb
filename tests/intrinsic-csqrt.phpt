--TEST--
Complex number square root test
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
	
	$c = csqrt($c);
	echo csqrt($a), "\n";
	echo sqrt($b), "\n";
	echo "$c\n";
	echo csqrt(array(0, 0)), "\n";
	echo csqrt(array(2.5, +INF)), "\n";
	echo csqrt(array(4.5, NAN)), "\n";
	echo csqrt(array(-INF, 1)), "\n";
	echo csqrt(array(+INF, 1)), "\n";
	echo csqrt(array(-INF, NAN)), "\n";
	echo csqrt(array(+INF, NAN)), "\n";
	echo csqrt(array(NAN, 0.5)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[1.7989, 1.1118]
[1.7989, 1.1118]
[[2.4987, -1.8009], [0.13857, 3.6082], [1.9405, -0.51533]]
[0, 0]
[INF, INF]
[NAN, NAN]
[0, INF]
[INF, 0]
[NAN, INF]
[INF, NAN]
[NAN, NAN]

