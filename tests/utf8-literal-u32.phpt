--TEST--
UTF8 literal test (32 bit)
--FILE--
<?php

/**
 * @engine qb
 * @param float64			$num
 * @local string<uint32> 	$text
 * @local uint32			$i
 * @local uint32			$c
 * @return void
 */
function test_function($num) {
	$text = "梁頌榮: $num";
	for($i = 0; $i < strlen($text); $i++) {
		$c = $text[$i];
		echo "$c\n";
	}
	echo $text, "\n";
}

test_function(3.14);

?>
--EXPECT--
26753
38924
27054
58
32
51
46
49
52
梁頌榮: 3.14
