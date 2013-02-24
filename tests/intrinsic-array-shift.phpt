--TEST--
Array shift test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4);
	echo array_shift($a), "\n";
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
1
[2, 3, 4]
