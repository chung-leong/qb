--TEST--
List construct test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[]	$a
 * @local	int32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3);
	list($b, $c, $d) = $a;
	echo "$b, $c, $d\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1, 2, 3
