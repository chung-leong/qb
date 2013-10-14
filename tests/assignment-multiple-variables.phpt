--TEST--
Multiple assignment test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$.*
 * @param	uint32	$a
 * @return	void
 * 
 */
function test_function($a) {
	$b = $c = $d = $a;
	$a = 77;
	echo "$a $b $c $d";
}

test_function(88);

?>
--EXPECT--
77 88 88 88
