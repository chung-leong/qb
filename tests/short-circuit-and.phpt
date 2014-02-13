--TEST--
And operator test
--FILE--
<?php

/**
 * @engine qb
 * @param bool $a
 * @param float $b
 * @return void
 */
function test_function($a, $b) {
	if($a && $b > 0) {
		echo "Good!\n";
	}
}

test_function(false, 0);
test_function(true, 0);
test_function(true, 1);

?>
--EXPECT--
Good!

