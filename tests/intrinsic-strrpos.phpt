--TEST--
Substring reverse search test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string $a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Lolita, light of my life, fire of my loins. My sin, my soul.";
	echo strrpos($a, "my"), "\n";
	echo strrpos($a, "my", -30), "\n";
	echo strrpos($a, "nose"), "\n";
}

test_function();

?>
--EXPECT--
52
17
-1