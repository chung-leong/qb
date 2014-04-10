--TEST--
Complex number inverse hyperbolic sine test
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
	$a = array(-2.3707, -2.8472);
	$c[0] = array(0, 0);
	$c[1] = array(0, NAN);
	$c[2] = array(3.5231, +INF);
	
	$c = casinh($c);
	echo casinh($a), "\n";
	echo "$c\n";
	echo casinh(array(4, 8)), "\n";
	echo casinh(array(4.5, NAN)), "\n";
	echo casinh(array(-INF, 8)), "\n";
	echo casinh(array(+INF, 12)), "\n";
	echo casinh(array(-INF, +INF)), "\n";
	echo casinh(array(+INF, +INF)), "\n";
	echo casinh(array(+INF, NAN)), "\n";
	echo casinh(array(NAN, 0.5)), "\n";
}

ini_set('precision', 8);
test_function();

?>
--EXPECT--
[-2, -0.8584]
[[0, 0], [NAN, NAN], [INF, 1.571]]
[2.8822897, 1.1046346]
[NAN, NAN]
[-INF, 0]
[INF, 0]
[-INF, 0.78539816]
[INF, 0.78539816]
[INF, NAN]
[NAN, NAN]

