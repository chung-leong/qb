--TEST--
Swizzle test (write)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[r,g,b,a]	$p
 * 
 * @return	void
 * 
 */
function test_function() {
	//$p = array(1, 2, 3, 4);
	//$p->rgb = 2;
	//echo "$p\n";
	//$p->ra = 1;
	//echo "$p\n";
	$p->bg = array(2, 3);
	//echo "$p\n";
}

test_function();

?>
--EXPECT--
[2, 2, 2, 4]
[1, 2, 2, 1]
[1, 3, 2, 1]
