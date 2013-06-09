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

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[1.7989\d+, 1.1117\d+\]
\[\[2.4986\d+, -1.8009\d+\], \[0.1385\d+, 3.6082\d+\], \[1.9405\d+, -0.5153\d+\]\]
