--TEST--
Increment array element test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[4]	$.*
 * 
 * @return	int32
 * 
 */
function test_function() {
	$a = array(1234, 5678, 9, 10);
	$a[2]++;
	$a[3] += 10;
	echo $a;
}

test_function();

?>
--EXPECT--
[1234, 5678, 10, 20]
