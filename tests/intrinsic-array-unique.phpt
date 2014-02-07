--TEST--
Unique array element test
--FILE--
<?php

/**
  * @engine	qb
  * @local	int32[]			$a
  * @local	float64[]		$b
  * @local	uint8[]			$c
  * @local	int32[12]		$d
  * @local	uint32[]		$e
  * @local	uint32[*][3]	$f
  */
function test_function() {
	$a = array(1, 2, 3, 4, 10, 4, 7, 1);
	$b = array(0.1, 4, 0.5, 0.1, 4);
	$c = array(245, 5, 5, 5, 1);
	$d = array(1, 6, 100, 101, 5, 6);
	$e = array(3, 14, 2);
	echo "$a\n";
	echo "$b\n";
	echo "$c\n";
	echo "$d\n";
	echo "$e\n";
	echo array_unique($a), "\n";
	echo array_unique($b), "\n";
	echo array_unique($c), "\n";
	echo array_unique($d), "\n";
	echo array_unique($e), "\n";
	
	echo "\n";
	$f[0] = array(1, 2, 3);
	$f[1] = array(1, 2, 4);
	$f[2] = array(1, 2, 3);
	$f[3] = array(4, 5, 6);
	$f[4] = array(1, 2, 3);
	$f[5] = array(1, 2, 3);
	$f[6] = array(1, 2, 3);
	$f[7] = array(1, 2, 4);	
	foreach(array_unique($f) as $e) {
		echo "$e\n";
	}
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 10, 4, 7, 1]
[0.1, 4, 0.5, 0.1, 4]
[245, 5, 5, 5, 1]
[1, 6, 100, 101, 5, 6, 1, 6, 100, 101, 5, 6]
[3, 14, 2]
[1, 2, 3, 4, 10, 7]
[0.1, 4, 0.5]
[245, 5, 1]
[1, 6, 100, 101, 5]
[3, 14, 2]

[1, 2, 3]
[1, 2, 4]
[4, 5, 6]
