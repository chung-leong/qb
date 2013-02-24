--TEST--
Memory-map file test (read-write)
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
function test_function(&$a) {
	$a[0] = 'A';
	$a[1] = 'G';
	$a[2] = 'N';
	$a[3] = 'E';
	$a[4] = 'S';
}

qb_compile();

$contents = str_repeat("\xFF\x7F\xAA\x00\xCC", 200);

$path = __FILE__ . ".dat";
file_put_contents($path, $contents);

$handle = fopen($path, "c+");

test_function($handle);

fclose($handle);

$contents = file_get_contents($path);

echo "Length: ", strlen($contents), "\n";
echo substr($contents, 0, 5);

unlink($path);


?>
--EXPECT--
Length: 1000
AGNES