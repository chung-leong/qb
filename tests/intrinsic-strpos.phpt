--TEST--
Substring search test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string $a
 * @local	string $b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Lolita, light of my life, fire of my loins. My sin, my soul.";
	$b = "sin";
	echo strpos($a, "fire"), "\n";
	echo strpos($a, $b), "\n";
	echo strpos($a, "nose"), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
26
47
-1