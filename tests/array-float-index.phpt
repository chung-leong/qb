--TEST--
Floating point array index test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]	$a
 * @local	float32		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$b = 1.5;
	$a[$b] = 5;
	$b = $a[$b];
	unset($a[1]);
	echo isset($a[$b]), "\n";
	echo "$a $b\n";
}

test_function();

?>
--EXPECT--
0
[0, 0, 0, 0] 5
