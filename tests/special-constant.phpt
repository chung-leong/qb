--TEST--
Special constant test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	if(defined("__QB__")) {
		$a = __QB__;
		echo "The constant __QB__ has the value $a\n";
	}
}

test_function();

?>
--EXPECT--
The constant __QB__ has the value 1
