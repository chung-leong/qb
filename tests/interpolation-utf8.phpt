--TEST--
Variable interpolation test (utf8)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	string<uint16>	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = "梁頌榮";
	$b = "ПИВО";
	echo "Strings: $a, $b";
}

test_function();

?>
--EXPECT--
Strings: 梁頌榮, ПИВО
