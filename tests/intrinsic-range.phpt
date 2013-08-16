--TEST--
Range function test
--FILE--
<?php

/**
  * @engine	qb
  * @local	int32[]	$a
  * @local	float64[]	$b
  * @local	uint8[]	$c
  * @local	int32[12]	$d
  * @local	uint32[]	$e
  */
function test_function() {
	$a = range(1, 10);
	$b = range(0.1, 4, 0.5);
	$c = range(245, 260, 1);
	$d = range(1, 6);
	$e = range(3, 14, 2);
	echo "$a\n";
	echo "$b\n";
	echo "$c\n";
	echo "$d\n";
	echo "$e\n";
}

qb_compile();
test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
[0.1, 0.6, 1.1, 1.6, 2.1, 2.6, 3.1, 3.6]
[245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4]
[1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6]
[3, 5, 7, 9, 11, 13]
