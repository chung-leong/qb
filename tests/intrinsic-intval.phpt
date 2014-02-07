--TEST--
Intval test
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
	$text = "123";
	$a = intval($text);

	$text = "12345";
	$b = intval($text);

	$text = "1234567890";
	$c = intval($text);

	$text = "1234567890123456";
	$d = intval($text);

	$text = "1234567890";
	$e = intval($text);
	
	echo "$a $b $c $d $e\n";
}

test_function();

?>
--EXPECT--
123 12345 1234567890 1234567890123456 1234567890
