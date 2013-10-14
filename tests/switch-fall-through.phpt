--TEST--
Switch statement test (fall through)
--FILE--
<?php

/**
 * @engine qb
 * @param uint32	$a
 */
function test_function($a) {
	switch($a) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 9: echo "Odd\n"; break;
		case 2:
		case 4:
		case 6:
		case 8:
		case 10: echo "Even\n"; break;
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
Odd
Even
Odd
Even
Other
