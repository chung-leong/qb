--TEST--
Complex number square root test
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
	$a = array(2, 4);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = csqrt($c);
	echo csqrt($a), "\n";
	echo "$c\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[1.7989, 1.1118]
[[2.4987, -1.8009], [0.13857, 3.6082], [1.9405, -0.51533]]
