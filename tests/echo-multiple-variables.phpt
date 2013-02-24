--TEST--
Echo multiple variables test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int16[2]	$c
 * @local	int32		$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 1234;
	$b = -777;
	$c = array( 5, 99 );
	echo $a, " ", $b, " ", $a + $b, " ", $c, " ", $c[1];
}

qb_compile();

test_function();

?>
--EXPECT--
1234 -777 457 [5, 99] 99
