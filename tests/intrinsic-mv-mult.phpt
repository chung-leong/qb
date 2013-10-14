--TEST--
Matrix-vector multiplication test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3][3]	$m
 * @local	float32[3]		$[ab]
 * @local	float32[2][3]	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$m = array(
		array(10,  0,  0),
		array( 0, 10,  0),
		array( 0,  0, 10),
	);
	$a = array(1, 2, 3);
	$b = mv_mult($m, $a);	
	echo "$b\n";
	
	$c[0] = array(1, 2, 3);
	$c[1] = array(4, 5, 6);
	echo mv_mult($m, $c), "\n";
}

ini_set("qb.column_major_matrix", true);

test_function();

?>
--EXPECT--
[10, 20, 30]
[[10, 20, 30], [40, 50, 60]]
