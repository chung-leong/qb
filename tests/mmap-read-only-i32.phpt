--TEST--
Memory-map file test (read-only, I32)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[]	$a
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

$contents = str_repeat("\xFF\x7F\xAA\x00\xCC\xAA\xBB\x01", 200);

$path = __FILE__ . ".dat";
file_put_contents($path, $contents);

$handle = fopen($path, "rb");
fseek($handle, 30, SEEK_SET);

test_function($handle);

fclose($handle);
unlink($path);

// assume little endian

?>
--EXPECT--
Length: 400
11173887
29076172
11173887
29076172
11173887
29076172
11173887
29076172
11173887
29076172
