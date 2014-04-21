--TEST--
UTF8 text argument test (32 bit)
--FILE--
<?php

/**
 * @engine qb
 * @param string<uint32> 	$text
 * @local uint32			$c
 * @local uint32			$i
 * @return void
 */
function test_function($text) {
	for($i = 0; $i < strlen($text); $i++) {
		$c = $text[$i];
		echo "$c\n";
	}
}

$text = "\xE6\xA2\x81\xE9\xA0\x8C\xE6\xA6\xAE";

test_function($text);

?>
--EXPECT--
26753
38924
27054
