--TEST--
Switch statement test (scalar)
--FILE--
<?php

/**
 * @engine qb
 * @param uint32	$a
 */
function test_function($a) {
	switch($a) {
		case 1: echo "One\n"; break;
		case 2: echo "Two\n"; break;
		case 3: echo "Three\n"; break;
		case 4: echo "Four\n"; break;
		default: echo "Other\n";
	}
}

test_function(1);
test_function(2);
test_function(3);
test_function(4);
test_function(42);

?>
--EXPECT--
One
Two
Three
Four
Other
