--TEST--
Argument by value test (multi-dimensional array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[4][4]	$a
 * @local	int32[4]	$row
 * 
 * @return	void
 * 
 */
function test_function($a) {
	foreach($a as $row) {
		echo "$row\n";
	}
}

qb_compile();

$a = array(
	range(1, 4),
	range(5, 8),
	range(9, 12),
	range(13, 16)
);

test_function($a);

?>
--EXPECT--
[1, 2, 3, 4]
[5, 6, 7, 8]
[9, 10, 11, 12]
[13, 14, 15, 16]
