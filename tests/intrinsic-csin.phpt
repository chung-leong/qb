--TEST--
Complex number sine test
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
	
	$c = csin($c);
	echo csin($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[24.83\d+, -11.3566\d+\]
\[\[571.75\d+, 4010.99\d+\], \[-0.6483\d+, 1.0664\d+\], \[-1.3197\d+, 3.3963\d+\]\]
