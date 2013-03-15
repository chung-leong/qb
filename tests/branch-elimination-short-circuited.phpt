--TEST--
Short-circuited conditional branch elimination test
--FILE--
<?php

/**
 * @engine qb
 * @local int32 $a
 */

function test_function() {
	$a = 5;
	if(1 == 1 && 2 == 2) {
		echo "Hello\n";
	} else {
		echo "Doh!\n";
	}
	if(1 == 2 || $a > 0) {
		echo "World\n";
	}
	if(1 == 2 || 5 == 9) {
		echo "Hell\n";
	}
}

qb_compile();

test_function();

?>
--EXPECT--
Hello
World
