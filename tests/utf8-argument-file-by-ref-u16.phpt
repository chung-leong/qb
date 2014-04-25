--TEST--
UTF-8 file by-ref argument test (16 bit)
--FILE--
<?php

$text = "Все счастливые семьи похожи друг на друга, каждая несчастливая семья несчастлива по-своему.\n";

$folder = dirname(__FILE__);
$text_path = "$folder/test.txt";
file_put_contents($text_path, $text);

/**
 * @engine qb
 * @param string<uint16> $text
 * @local uint32		 $i
 */
function test_function(&$text) {
	for($i = 0; $i < strlen($text); $i++) {
		if($text[$i] >= 0x0430 && $text[$i] <= 0x044F) {
			$text[$i] -= 0x20;
		}
	}
}

$f = fopen($text_path, "r+");
test_function($f);
fclose($f);

$after = file_get_contents($text_path);
echo $after;

unlink($text_path);

?>
--EXPECT--
ВСЕ СЧАСТЛИВЫЕ СЕМЬИ ПОХОЖИ ДРУГ НА ДРУГА, КАЖДАЯ НЕСЧАСТЛИВАЯ СЕМЬЯ НЕСЧАСТЛИВА ПО-СВОЕМУ.

