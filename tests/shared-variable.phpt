--TEST--
Shared variable test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @shared	int32[4][4]		$a
 * @local	uint32	 		$id
 * 
 * @return	void
 * 
 */
function test_function() {
	$id = fork(4);
	$a[$id] = $id;
	spoon();
	
	echo "$a\n";
}

test_function();

?>
--EXPECT--
[[0, 0, 0, 0], [1, 1, 1, 1], [2, 2, 2, 2], [3, 3, 3, 3]]
