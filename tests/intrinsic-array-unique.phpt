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
	
	$f[] = array(1, 2, 3);
	$f[] = array(1, 2, 4);
	$f[] = array(1, 2, 3);
	$f[] = array(4, 5, 6);
	$f[] = array(1, 2, 3);
	$f[] = array(1, 2, 3);
	$f[] = array(1, 2, 3);
	$f[] = array(1, 2, 4);
	echo array_unique($f), "\n";
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
[[1, 2, 3], [1, 2, 4], [4, 5, 6]]
