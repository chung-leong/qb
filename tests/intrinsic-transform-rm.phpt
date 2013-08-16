--TEST--
Vector transform test (row major)
--FILE--
<?php

/**
  * @engine qb
  * @local float32[2][3]	$m2
  * @local float32[3][4]	$m3
  * @local float32[4][5]	$m4
  * @local float32[2]		$v2
  * @local float32[3]		$v3
  * @local float32[4]		$v4
  * @local float32[2][2]	$a2
  * @local float32[2][3]	$a3
  * @local float32[2][4]	$a4
  */
function test_function() {
	$m2 = array(
		array(2, 0, 0.1),
		array(0, 2, 0.2),
	);
	$m3 = array(
		array(3, 0, 0, 0.1),
		array(0, 3, 0, 0.2),
		array(0, 0, 3, 0.3),
	);
	$m4 = array(
		array(4, 0, 0, 0, 0.1),
		array(0, 4, 0, 0, 0.2),
		array(0, 0, 4, 0, 0.3),
		array(0, 0, 0, 4, 0.4),
	);
	
	$v2 = array(1, 1);
	$v3 = array(1, 1, 1);
	$v4 = array(1, 1, 1, 1);
	
	$a2 = array(
		array(1, 1),
		array(1, 2),
	);
	$a3 = array(
		array(1, 1, 1),
		array(1, 2, 3),
	);
	$a4 = array(
		array(1, 1, 1, 1),
		array(1, 2, 3, 4),
	);
	
	echo transform($m2, $v2), "\n";
	echo transform($m3, $v3), "\n";
	echo transform($m4, $v4), "\n";
	
	echo transform($m2, $a2), "\n";
	echo transform($m3, $a3), "\n";
	echo transform($m4, $a4), "\n";
}

ini_set('qb.column_major_matrix', false);
qb_compile();
test_function();

?>
--EXPECT--
[2.1, 2.2]
[3.1, 3.2, 3.3]
[4.1, 4.2, 4.3, 4.4]
[[2.1, 2.2], [2.1, 4.2]]
[[3.1, 3.2, 3.3], [3.1, 6.2, 9.3]]
[[4.1, 4.2, 4.3, 4.4], [4.1, 8.2, 12.3, 16.4]]
