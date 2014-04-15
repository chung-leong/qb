--TEST--
Complex number log test
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
	
	$c = clog($c);
	echo clog($a), "\n";
	echo log($b), "\n";
	echo "$c\n";
	echo clog(array(-1.0 * 0, 0)), "\n";
	echo clog(array(+0.0, 0)), "\n";
	echo clog(array(4.5, +INF)), "\n";
	echo clog(array(4.5, NAN)), "\n";
	echo clog(array(-INF, 8)), "\n";
	echo clog(array(+INF, 12)), "\n";
	echo clog(array(-INF, +INF)), "\n";
	echo clog(array(+INF, +INF)), "\n";
	echo clog(array(+INF, NAN)), "\n";
	echo clog(array(NAN, +INF)), "\n";
	echo clog(array(NAN, 0.5)), "\n";
}

test_function();

?>
--EXPECT--
[1.497866, 1.107149]
[1.497866, 1.107149]
[[2.249905, -1.249046], [2.567899, 3.064821], [1.394046, -0.5191461]]
[-INF, 3.1415926535898]
[-INF, 0]
[INF, 1.5707963267949]
[NAN, NAN]
[INF, 3.1415926535898]
[INF, 0]
[INF, 2.3561944901923]
[INF, 0.78539816339745]
[INF, NAN]
[INF, NAN]
[NAN, NAN]

