--TEST--
Complex number pow test
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
	
	$c = cpow($c, array(2, 0));
	echo cpow($a, array(3,-3)), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
\[961.72\d+, -2283.32\d+\]
\[\[-7(2(\.00\d*)?|1.99\d+), -5(4(\.00\d*)?|3.99\d+)\], \[16(8(\.00\d*)?|7.99\d+), -26\], \[8.2(5\d*|499\d+), -1(4(\.00\d*)?|3.99\d+)\]\]
