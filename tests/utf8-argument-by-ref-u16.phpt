--TEST--
UTF8 text by-ref argument test (16 bit)
--FILE--
<?php

/**
 * @engine qb
 * @param string<uint16> 	$text[12]
 * @local uint32			$i
 * @return void
 */
function test_function(&$text1, &$text2) {
	$text2 = $text1;
	for($i = 0; $i < strlen($text1); $i++) {
		$text1[$i]++;
	}
}

$text1 = "\xE6\xA2\x81\xE9\xA0\x8C\xE6\xA6\xAE";

echo "$text1\n";

test_function($text1, $text2);

echo "$text1\n$text2\n";

?>
--EXPECT--
梁頌榮
梂頍榯
梁頌榮
