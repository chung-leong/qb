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
	
	$c = csinh($c);
	echo csinh($a), "\n";
	echo "$c\n";
}

qb_compile();
ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-2.3707, -2.8472]
[[-9.1276, -4.1491], [-1.1952E+5, 1.8614E+5], [-6.8842, -15.07]]
