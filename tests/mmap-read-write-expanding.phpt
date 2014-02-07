--TEST--
Memory-map file test (read-write, expanding)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	uint8[*]	$a
 * @local	uint32		$i
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
	echo $a, "\n";
}


$path = __FILE__ . ".dat";
@unlink($path);
$handle = fopen($path, "w+");

test_function($handle);
fclose($handle);

$contents = file_get_contents($path);
echo "Length: ", strlen($contents), "\n";
echo "Contents: ", $contents, "\n";

unlink($path);

?>
--EXPECT--
[65, 71, 78, 69, 83]
Length: 5
Contents: AGNES
