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
--EXPECT--
[-0.0005079806, 1.000439]
[[-8.510985E-9, -1], [-0.1729505, 0.8225824], [0.02341185, -0.9724818]]
