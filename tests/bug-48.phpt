--TEST--
Bug #48
--FILE--
<?php

$folder = dirname(__FILE__);
$path = "{$folder}/bug-48.dat";
file_put_contents($path, "\x01\x02\x03\x04");

$a = fopen($path, "r");

/**
 * @engine	qb 
 * @global	int8[]	$a
 */
function test() {
	global $a;
	echo $a[0], $a[1], $a[2], $a[3], "\n";
}

test();
test();

?>
--EXPECT--
1234
1234

