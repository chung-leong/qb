--TEST--
UTF-8 file argument test (32-bit)
--FILE--
<?php

$text = "幸福的家庭都是相似的，不幸的家庭各有各的不幸。\n";

$folder = dirname(__FILE__);
$text_path = "$folder/test.txt";
file_put_contents($text_path, $text);

/**
 * @engine qb
 * @param string<uint32> $text
 */
function test_function($text) {
	echo $text[0] + 0, "\n";
	echo $text[1] + 0, "\n";
	echo $text;
}

$f = fopen($text_path, "r");
test_function($f);
fclose($f);

unlink($text_path);

?>
--EXPECT--
24184
31119
幸福的家庭都是相似的，不幸的家庭各有各的不幸。

