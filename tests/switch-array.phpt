--TEST--
Switch statement test (array)
--FILE--
<?php

/**
 * @engine qb
 * @param uint32[2]	$a
 */
function test_function($a) {
	switch($a) {
		case array(1, 2): echo "One-Two\n"; break;
		case array(2, 3): echo "Two-Three\n"; break;
		case array(3, 4): echo "Three-Four\n"; break;
		case array(4, 5): echo "Four-Five\n"; break;
		default: echo "Other\n";
	}
}

qb_compile();

test_function(array(1, 2));
test_function(array(2, 3));
test_function(array(3, 4));
test_function(array(4, 5));
test_function(array(1, 42));

?>
--EXPECT--
One-Two
Two-Three
Three-Four
Four-Five
Other
