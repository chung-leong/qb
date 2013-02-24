--TEST--
Array expansion test (multi-dimensional)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*][4]	$a
 * @local	int32[4]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[] = array(1, 2, 3, 4);
	$a[] = array(2, 3, 4, 5);
	$a[] = array(3, 4, 5, 6);
	$a[] = array(4, 5, 6, 7);
	
	foreach($a as $b) {
		echo "$b\n";
	}
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 2, 3, 4]
[2, 3, 4, 5]
[3, 4, 5, 6]
[4, 5, 6, 7]
