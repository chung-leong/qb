--TEST--
Sort test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(3, 1, 7, 9, 4, 5, 2, 6, 8);
	echo "$a\n";
	sort($a);
	echo "$a\n";
	rsort($a);
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[3, 1, 7, 9, 4, 5, 2, 6, 8]
[1, 2, 3, 4, 5, 6, 7, 8, 9]
[9, 8, 7, 6, 5, 4, 3, 2, 1]
