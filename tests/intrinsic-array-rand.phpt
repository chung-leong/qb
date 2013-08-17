--TEST--
Random array element test
--FILE--
<?php

/**
  * @engine	qb
  * @local	int32[]	$a
  * @local	int32	$b
  */
function test_function() {
	$a = array(10, 20, 30, 40, 50, 60, 70, 80);
	$b = 1;
	echo array_rand($a, 1), "\n";
	echo array_rand($a, 1), "\n";
	echo array_rand($a, 2), "\n";
	echo array_rand($a, $b), "\n";
}

qb_compile();
test_function();

?>
--EXPECTREGEX--
[0-7] 
[0-7]
\[[0-7], [0-7]\]
\[[0-7]\]
