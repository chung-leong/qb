--TEST--
Missing type declaration test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$[km]
 * @local	int32	$a
 * 
 * @return	void
 * 
 */
function test_function($k, $m) {
	$a = 5;
	// comment
	$b = 6;
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
Fatal error: Missing type declaration: b in .+ \d+