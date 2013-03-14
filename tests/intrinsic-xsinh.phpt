--TEST--
Complex number hyperbolic sine test
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
	
	$c = xsinh($c);
	echo xsinh($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[-2.370674, -2.847239]
[[-9.127589, -4.14907], [-119518.5, 186139], [-6.884162, -15.06963]]
