--TEST--
Complex number tangent test
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
	
	$c = ctan($c);
	echo ctan($a), "\n";
	echo tan($b), "\n";
	echo "$c\n";
	echo ctan(array(4.5, NAN)), "\n";
	echo ctan(array(-INF, 8)), "\n";
	echo ctan(array(+INF, 12)), "\n";
	echo ctan(array(-INF, +INF)), "\n";
	echo ctan(array(+INF, +INF)), "\n";
	echo ctan(array(+INF, NAN)), "\n";
	echo ctan(array(NAN, 0.5)), "\n";
	echo ctan(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-0.00050798, 1.0004]
[-0.00050798, 1.0004]
[[-8.511E-9, -1], [-0.17295, 0.82258], [0.023412, -0.97248]]
[NAN, NAN]
[NAN, NAN]
[NAN, NAN]
[-0, 1]
[0, 1]
[NAN, NAN]
[NAN, NAN]
[0, 1]

