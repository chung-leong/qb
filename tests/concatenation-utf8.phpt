--TEST--
String concatenation test (utf8)
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
	echo $a . " ♥ " . $b . " \n";
}

test_function();

?>
--EXPECT--
梁頌榮 ♥ ПИВО
