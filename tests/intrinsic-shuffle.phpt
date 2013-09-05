--TEST--
Array shuffle test
--FILE--
<?php

/**
  * @engine	qb
  * @local	int32[]			$a
  * @local	float64[][3]	$b
  */
function test_function() {
	$a = array(1, 2, 3, 4, 5, 6, 7, 8);
	shuffle($a);
	echo "$a\n";
	$b = array(
		array(1, 1, 1),
		array(2, 2, 2),
		array(3, 3, 3),
		array(4, 4, 4),
		array(5, 5, 5),
		array(6, 6, 6),
		array(7, 7, 7),
		array(8, 8, 8),
	);
	shuffle($b);
	echo "$b\n";
}

qb_compile();
test_function();

?>
--EXPECTREGEX--
\[[1-8], [1-8], [1-8], [1-8], [1-8], [1-8], [1-8], [1-8]\]
\[\[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\], \[[1-8], [1-8], [1-8]\]\]
