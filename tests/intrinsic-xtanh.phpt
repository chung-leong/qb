--TEST--
Complex number hyperbolic tangent test
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
	
	$c = ctanh($c);
	echo ctanh($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[1.0046\d+, 0.03642\d+\]
\[\[0.9967\d+, 0.0037\d+\], \[-1, 9.2913\d+E-12\], \[1.0011\d+, 0.0013\d+\]\]
