--TEST--
Random array element test
--FILE--
<?php

/**
  * @engine	qb
  * @local	int32[]		$a
  * @local	int32		$b
  * @local	int32[*][2]	$c
  */
function test_function() {
	$a = array(10, 20, 30, 40, 50, 60, 70, 80);
	$b = 1;
	echo array_rand($a, 1), "\n";
	echo array_rand($a, 1), "\n";
	echo array_rand($a, 2), "\n";
	echo array_rand($a, $b), "\n";
	
	$c[] = array(1, 2);
	$c[] = array(3, 4);
	$c[] = array(5, 6);
	$c[] = array(7, 8);
	echo array_rand($c, 1), "\n";
	echo array_rand($c, 2), "\n";
}

qb_compile();
test_function();

?>
--EXPECTREGEX--
[0-7]
[0-7]
\[[0-7], [0-7]\]
\[[0-7]\]
[0-3]
\[[0-3], [0-3]\]
