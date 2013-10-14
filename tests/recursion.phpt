--TEST--
Recursion test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "a = $a\n";
	if($a < 5) {
		test_function($a + 1);
	}
}

test_function(0);

?>
--EXPECT--
a = 0
a = 1
a = 2
a = 3
a = 4
a = 5
