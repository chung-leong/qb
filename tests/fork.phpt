--TEST--
Fork test
--FILE--
<?php

/**
 * @engine qb
 * @param int32		$c
 * @local string	$text
 * @local int32		$.*
 */
function test_function($c) {
	$a = 88;
	$b = 8;
	$text = "Hello";
	list($fork_id, $fork_count) = fork($c);
	echo "$text: $a, $b, $c, $fork_id, $fork_count\n";
}

test_function(6);

?>
--EXPECT--
Hello: 88, 8, 6, 0, 6
Hello: 88, 8, 6, 1, 6
Hello: 88, 8, 6, 2, 6
Hello: 88, 8, 6, 3, 6
Hello: 88, 8, 6, 4, 6
Hello: 88, 8, 6, 5, 6
