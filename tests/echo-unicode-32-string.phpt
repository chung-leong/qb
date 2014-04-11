--TEST--
Echo unicode string (32-bit)
--FILE--
<?php

/**
 * @engine qb
 * @local string<uint32>	$a 
 */
function test () {
	$a[] = 0x006D;
	$a[] = 0x0069;
	$a[] = 0x0142;
	$a[] = 0x006F;
	$a[] = 0x015B;
	$a[] = 0x0107;
	echo $a, "\n";
}

test();

?>
--EXPECT--
miłość
