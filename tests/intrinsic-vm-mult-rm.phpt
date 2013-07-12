--TEST--
Vector-matrix multiplication test (row-major)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][2]		$m2
 * @local	float32[3][3]		$m3
 * @local	float32[4][4]		$m4
 * @local	float32[5][5]		$m5
 * @local	float32[2]			$(v2|r2)
 * @local	float32[3]			$(v3|r3)
 * @local	float32[4]			$(v4|r4)
 * @local	float32[5]			$(v5|r5)
 * 
 * @return	void
 * 
 */
function test_function() {
	$m2 = array(
		array(1, 2),
		array(3, 4),
	);
	$v2 = array(1, 2);
	$r2 = vm_mult_rm($v2, $m2);
	echo "$r2\n";
	
	$m3 = array(
		array(1, 2, 3),
		array(4, 5, 6),
		array(7, 8, 9),
	);
	$v3 = array(1, 2, 3);
	$r3 = vm_mult_rm($v3, $m3);
	echo "$r3\n";
	
	$m4 = array(
		array( 1,  2,  3,  4),
		array( 5,  6,  7,  8),
		array( 9, 10, 11, 12),
		array(13, 14, 15, 16),
	);
	$v4 = array(1, 2, 3, 4);
	$r4 = vm_mult_rm($v4, $m4);
	echo "$r4\n";
	
	$m5 = array(
		array( 1,  2,  3,  4,  5),
		array( 6,  7,  8,  9, 10),
		array(11, 12, 13, 14, 15),
		array(16, 17, 18, 19, 20),
		array(21, 22, 23, 24, 25),
	);
	$v5 = array(1, 2, 3, 4, 5);
	$r5 = vm_mult_rm($v5, $m5);
	echo "$r5\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[7, 10]
[30, 36, 42]
[90, 100, 110, 120]
[215, 230, 245, 260, 275]
