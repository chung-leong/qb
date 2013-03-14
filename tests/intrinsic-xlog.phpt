--TEST--
Complex number log test
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
	
	$c = xlog($c);
	echo xlog($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
