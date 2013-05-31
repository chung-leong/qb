<?php

/**
 * @engine qb
 * @local int32[4] $b
 */
function test() {
	$b = array(1, 2, 3, 4);
	$b--;
	echo "$b\n";
}

qb_compile();
test();

?>