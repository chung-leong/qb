--TEST--
Array slice test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*]	$a
 * @local	uint32		$[bc]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4, 5, 6, 7);
	$b = 2;
	$c = 3;
	echo array_slice($a, 0), "\n";
	echo array_slice($a, 1), "\n";
	echo array_slice($a, 2, 3), "\n";
	echo array_slice($a, $b), "\n";
	echo array_slice($a, $b, $c), "\n";
	echo array_slice($a, $a[$b], $a[$b]), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5, 6, 7]
[2, 3, 4, 5, 6, 7]
[3, 4, 5]
[3, 4, 5, 6, 7]
[3, 4, 5]
[4, 5, 6]
