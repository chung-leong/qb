--TEST--
Array element isset test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]	$a
 * @local	uint32		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$b = 1;
	$a[$b] = 5;
	echo isset($a[1]), "\n";
	echo isset($a[$b]), "\n";
	echo isset($a[0]), "\n";
	echo isset($a[$b + 8]), "\n";
}

test_function();

?>
--EXPECT--
1
1
0
0
