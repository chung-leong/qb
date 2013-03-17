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
	
	$c = clog($c);
	echo clog($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1.497866, 1.107149]
[[2.249905, -1.249046], [2.567899, 3.064821], [1.394046, -0.5191461]]
