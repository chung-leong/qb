--TEST--
Swizzle test
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
	$p = array(1, 2, 3, 4);
	echo "$p->rgb\n";
	echo "$p->rrrr\n";
	echo "$p->abgr\n";
	echo "$p->ggrraa\n";
}

test_function();

?>
--EXPECT--
[1, 2, 3]
[1, 1, 1, 1]
[4, 3, 2, 1]
[2, 2, 1, 1, 4, 4]
