--TEST--
UTF-8 file by-ref argument test (32 bit)
--FILE--
<?php

$text = "幸福的家庭都是相似的，不幸的家庭各有各的不幸。\n";

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
		if($text[$i] == '的') {
			$text[$i] = '嘅';
		} else if($text[$i] == '是') {
			$text[$i] = '係';
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
幸福嘅家庭都係相似嘅，不幸嘅家庭各有各嘅不幸。

