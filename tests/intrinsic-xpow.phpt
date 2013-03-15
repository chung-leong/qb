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
	
	$c = xpow($c, array(2, 0));
	echo xpow($a, array(3,-3)), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[961.7267, -2283.324]
[[-72, -54], [168, -26], [8.25, -14]]
