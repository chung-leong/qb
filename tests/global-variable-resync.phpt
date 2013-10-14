--TEST--
Global variable resync test
--FILE--
<?php

$a = 100;

/**
 * A test function
 * 
 * @engine	qb
 * @global	int32	$a
 * 
 * @return	void
 * 
 */
function other_function() {
	global $a;
	echo "$a\n";
	$a = 102;
}

function test_function() {
	global $a;
	echo "$a\n";
	$a = 101;
	other_function();
	echo "$a\n";
	$a = 103;
}

test_function();
echo "$a\n";

?>
--EXPECT--
100
101
102
103
