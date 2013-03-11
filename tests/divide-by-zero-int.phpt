--TEST--
Division by zero test (integer)
--FILE--
<?php

/**
 * @engine qb
 * @local int32 $\w+
 */
function test_function() {
	$a = 1;
	$b = $a / 0;
	echo $b;
}

qb_compile();

test_function();

?>
--EXPECTREGEX--
.*Division by zero.*line 9