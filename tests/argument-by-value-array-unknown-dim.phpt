--TEST--
Argument by value test (array, undefined dimensions)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[][]	$a
 * @local	uint32		$.*
 * 
 * @return	void
 * 
 */
function test_function($a) {
	for($i = 0; $i < count($a); $i++) {
		echo $a[$i], "\n";
	}
}

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
