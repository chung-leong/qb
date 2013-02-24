--TEST--
Matrix multiplication test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3][3]	$m
 * @local	float32[3]		$[ab]
 * 
 * @return	void
 * 
 */
function test_function() {
	$m = array(
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
	);
	$a = array(100, 200, 300);
	$b = mmult($m, $a);
	
	echo "$b\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1400, 3200, 5000]
