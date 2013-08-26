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
 * @local	int32[4][2]	$d
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
	
	$d = array(array(1, 2), array(3, 4), array(5, 6), array(7, 8));
	echo array_slice($d, 1), "\n";
	echo array_slice($d, 1, $b), "\n";
	$b = 1;
	echo array_slice($d, $a[$b], $a[$b]), "\n";
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
[[3, 4], [5, 6], [7, 8]]
[[3, 4], [5, 6]]
[[5, 6], [7, 8]]
