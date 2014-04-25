--TEST--
UTF-8 file argument test (16-bit)
--FILE--
<?php

$text = "Все счастливые семьи похожи друг на друга, каждая несчастливая семья несчастлива по-своему.\n";

$folder = dirname(__FILE__);
$text_path = "$folder/test.txt";
file_put_contents($text_path, $text);

/**
 * @engine qb
 * @param string<uint16> $text
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
1042
1089
Все счастливые семьи похожи друг на друга, каждая несчастливая семья несчастлива по-своему.

