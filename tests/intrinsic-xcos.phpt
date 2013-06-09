--TEST--
Complex number cosine test
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
	
	$c = ccos($c);
	echo ccos($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[-11.3642\d+, -24.81\d+\]
\[\[-4010.\d+, 571.75\d+\], \[1.4002\d+, 0.4937\d+\], \[-3.523\d+, -1.2722\d+\]\]
