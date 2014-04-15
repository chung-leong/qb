--TEST--
Complex number inverse hyperbolic cosine test
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
	$a = $b = array(119518.5, -186139);
	$c[0] = array(0, 0);
	$c[1] = array(-13, +INF);
	$c[2] = array(3.5, NAN);
	
	$c = cacosh($c);
	echo cacosh($a), "\n";
	echo acosh($b), "\n";
	echo "$c\n";
	echo casinh(array(0.4, 0.8)), "\n";
	echo cacosh(array(0, 0)), "\n";
	echo cacosh(array(5, +INF)), "\n";
	echo cacosh(array(12, NAN)), "\n";
	echo cacosh(array(-INF, 8)), "\n";
	echo cacosh(array(+INF, 7)), "\n";
	echo cacosh(array(-INF, +INF)), "\n";
	echo cacosh(array(+INF, +INF)), "\n";
	echo cacosh(array(INF, NAN)), "\n";
	echo cacosh(array(NAN, 0)), "\n";
	echo cacosh(array(NAN, +INF)), "\n";
}

test_function();

?>
--EXPECT--
[13, -0.9999999]
[13, -0.9999999]
[[0, 1.570796], [INF, 1.570796], [NAN, NAN]]
[0.53321829058411, 0.7730863567195]
[0, 1.5707963267949]
[INF, 1.5707963267949]
[NAN, NAN]
[INF, 3.1415926535898]
[INF, 0]
[INF, 2.3561944901923]
[INF, 0.78539816339745]
[INF, NAN]
[NAN, NAN]
[INF, NAN]
