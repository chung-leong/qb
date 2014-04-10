--TEST--
Bug 36
--FILE--
<?php

/** @engine qb */
function test() {
	echo ctan(array(2, 3)) + array(0, 0), "\n";
}

ini_set('precision', 8);
test();

?>
--EXPECT--
[-0.0037640256, 1.0032386]

