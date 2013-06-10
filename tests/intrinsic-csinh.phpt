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

test_function();

?>
--EXPECTREGEX--
\[-2.3706\d+, -2.8472\d+\]
\[\[-9.1275\d+, -4.1490\d+\], \[-119518.\d+, 1861\d+\], \[-6.8841\d+, -15.0696\d+\]\]
