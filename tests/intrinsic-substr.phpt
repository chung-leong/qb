--TEST--
Substring test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string		$a
 * @local	uint32		$[bc]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "Hello world";
	$b = 2;
	$c = 3;
	echo substr($a, 0), "\n";
	echo substr($a, 1), "\n";
	echo substr($a, 2, 3), "\n";
	echo substr($a, $b), "\n";
	echo substr($a, $b, $c), "\n";
}

test_function();

?>
--EXPECT--
Hello world
ello world
llo
llo world
llo
