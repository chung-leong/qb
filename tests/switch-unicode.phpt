--TEST--
Unicode switch statement
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	string<uint16>	$text
 * @local	uint32			$i
 * @return	void
 * 
 */
function test_function($text) {
	for($i = 0; $i < strlen($text); $i++) {
		switch($text[$i]) {
			case '梁':
				echo "Leong ";
				break;
			case '頌':
				echo "Chung ";
				break;
			case '榮':
				echo "Wing ";
				break;
		}
	}
	echo "\n";
}

$text = "\xE6\xA2\x81\xE9\xA0\x8C\xE6\xA6\xAE";

test_function($text);

?>
--EXPECT--
Leong Chung Wing 
