--TEST--
Array resize in forked paths test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][]		$a
 * @local	uint32	 		$id
 *
 * @shared	int32[][]		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$id = fork(4);
	array_resize($a, 2, 2);
	spoon();
	
	array_resize($b, 2, 2);
	
	$id = fork(4);
	array_resize($b, 2, 2);	
}

test_function();

?>
--EXPECTREGEX--
.*Fatal error.*line 23
