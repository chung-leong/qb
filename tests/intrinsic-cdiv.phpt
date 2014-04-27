--TEST--
Complex number division test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]			$a
 * @local	float32[2]			$b
 * @local	float64[3][2]		$c
 * @local	complex<float32>	$e
 * @local	complex<float32>	$f
 *
 * @return	void
 *
 */
function test_function() {
	$a = $e = array(2, 4);
	$b = $f = array(8, -7);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = cdiv($c, $b);
	echo $a / $b, "\n";
	echo cdiv($a, $b), "\n";
	echo $e / $f, "\n";
	echo "$c\n";
}

test_function();

?>
--EXPECT--
[0.25, -0.5714286]
[-0.1061947, 0.4070796]
[-0.1061947, 0.4070796]
[[0.76991150442478, -0.45132743362832], [-0.98230088495575, -0.73451327433628], [0.3716814159292, 0.075221238938053]]

