--TEST--
Memory-map file test (read-only, U08)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint8[]	$a
 * @local	uint32	$i
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "Length: ", sizeof($a), "\n";
	for($i = 0; $i < 10; $i++) {
		echo "{$a[$i]}\n";
	}
}

$contents = str_repeat("\xFF\x7F\xAA\x00\xCC", 200);

$path = __FILE__ . ".dat";
file_put_contents($path, $contents);

$handle = fopen($path, "rb");
fseek($handle, 30, SEEK_SET);

test_function($handle);

echo ftell($handle);

fclose($handle);
unlink($path);


?>
--EXPECT--
Length: 1000
255
127
170
0
204
255
127
170
0
204
30