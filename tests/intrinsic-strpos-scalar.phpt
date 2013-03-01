--TEST--
Substring search test (scalar)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string $a
 * @local	char $b
 * @local	int32	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Lolita, light of my life, fire of my loins. My sin, my soul.";
	$b = 'o';
	$c = 0x61;	// 'a'
	echo strpos($a, $b), "\n";
	echo strpos($a, $c), "\n";
	echo strpos($a, 100), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
1
5
-1