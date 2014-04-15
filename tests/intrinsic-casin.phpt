--TEST--
Complex number inverse sine test
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
	$a = $b = array(-1.3197, 3.3964);
	$c[0] = array(0, 0);
	$c[1] = array(0, NAN);
	$c[2] = array(3.5231, +INF);
	
	$c = casin($c);
	echo casin($a), "\n";
	echo asin($b), "\n";
	echo "$c\n";
	echo casin(array(4.5, NAN)), "\n";
	echo casin(array(-INF, 8)), "\n";
	echo casin(array(+INF, 12)), "\n";
	echo casin(array(-INF, +INF)), "\n";
	echo casin(array(+INF, +INF)), "\n";
	echo casin(array(+INF, NAN)), "\n";
	echo casin(array(NAN, 0.5)), "\n";
	echo casin(array(NAN, +INF)), "\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-0.3584, 2]
[-0.3584, 2]
[[0, 0], [0, NAN], [0, INF]]
[NAN, NAN]
[-1.570796327, INF]
[1.570796327, INF]
[-0.7853981634, INF]
[0.7853981634, INF]
[NAN, INF]
[NAN, NAN]
[NAN, INF]

