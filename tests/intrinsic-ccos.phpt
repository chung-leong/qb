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
ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-11.364, -24.815]
[[-4011, 571.75], [1.4003, 0.49378], [-3.5231, -1.2722]]
