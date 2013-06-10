--TEST--
Complex number exponential test
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
	
	$c = cexp($c);
	echo cexp($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[-4.8298\d+, -5.5920\d+\]
\[\[-18.3005\d+, -8.2776\d+\], \[1.2212\d+E-6, 1.9020\d+E-6\], \[-13.78\d+, -30.11\d+\]\]
