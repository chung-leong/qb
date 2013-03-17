--TEST--
Complex number absolute value test
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
	
	echo cabs($a), "\n";
	echo cabs($c), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
4.472136
[9.486833, 13.0384, 4.031129]
