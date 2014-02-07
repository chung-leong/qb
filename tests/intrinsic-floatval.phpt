--TEST--
Floatval test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int8	$a
 * @local	int16	$b
 * @local	uint32	$c
 * @local	uint64	$d
 * @local	float64	$e
 * @local	string	$text
 * 
 * @return	void
 * 
 */
function test_function() {
	$text = "123.67";
	$a = floatval($text);

	$text = "12345.67";
	$b = floatval($text);

	$text = "1234567890.67";
	$c = floatval($text);

	$text = "1234567890123456.67";
	$d = floatval($text);

	$text = "1234567890.67";
	$e = doubleval($text);
	
	echo "$a $b $c $d $e\n";
}

test_function();

?>
--EXPECT--
123 12345 1234567890 1234567890123456 1234567890.67
