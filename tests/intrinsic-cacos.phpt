--TEST--
Complex number inverse cosine test
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
	$a = $b = array(-11.364, -24.815);
	$c[0] = array(0, 0);
	$c[1] = array(0, NAN);
	$c[2] = array(3.5231, +INF);
	
	$c = cacos($c);
	echo cacos($a), "\n";
	echo acos($b), "\n";
	echo "$c\n";
	echo cacos(array(4.5, NAN)), "\n";
	echo cacos(array(-INF, 8)), "\n";
	echo cacos(array(+INF, 12)), "\n";
	echo cacos(array(-INF, +INF)), "\n";
	echo cacos(array(+INF, +INF)), "\n";
	echo cacos(array(+INF, NAN)), "\n";
	echo cacos(array(NAN, 0.5)), "\n";
	echo cacos(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[2, 4]
[2, 4]
[[1.5708, -0], [1.5708, NAN], [1.5708, -INF]]
[NAN, NAN]
[3.141592654, -INF]
[0, -INF]
[2.35619449, -INF]
[0.7853981634, -INF]
[NAN, -INF]
[NAN, NAN]
[NAN, -INF]

