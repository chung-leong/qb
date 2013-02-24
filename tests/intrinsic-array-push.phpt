--TEST--
Array push test
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
	$a = array(1, 2, 3);
	echo array_push($a, 3 + 1), "\n";
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
4
[1, 2, 3, 4]
