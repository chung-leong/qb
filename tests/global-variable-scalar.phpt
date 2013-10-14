--TEST--
Global variable test (scalar)
--FILE--
<?php

$a = 1968;

/**
 * A test function
 * 
 * @engine	qb
 * @global	int32	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	global $a;
	echo "Inside qb: $a\n";
	$a = 2021;
}

test_function();

echo "Outside qb: $a\n";

?>
--EXPECT--
Inside qb: 1968
Outside qb: 2021
