--TEST--
Complex number tangent test
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
	
	$c = ctan($c);
	echo ctan($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[-0.0005\d+, 1.0004\d+\]
\[\[-8.5109\d+E-9, -(1(\.0000\d+)?|0\.9999\d+)\], \[-0.1729\d+, 0.8225\d+], \[0.0234\d+, -0.9724\d+\]\]
