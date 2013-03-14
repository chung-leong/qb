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
	
	$c = xcos($c);
	echo xcos($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[-11.36423, -24.81465]
[[-4010.996, 571.7536], [1.400264, 0.4937808], [-3.523133, -1.272242]]
