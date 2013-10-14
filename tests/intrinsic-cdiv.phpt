--TEST--
Complex number division test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]		$a
 * @local	float32[2]		$b
 * @local	float32[3][2]	$c
 *
 * @return	void
 *
 */
function test_function() {
	$a = array(2, 4);
	$b = array(8, -7);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = cdiv($c, $b);
	echo cdiv($a, $b), "\n";
	echo "$c\n";
}

test_function();

?>
--EXPECT--
[-0.1061947, 0.4070796]
[[0.7699115, -0.4513274], [-0.9823009, -0.7345133], [0.3716814, 0.07522124]]
