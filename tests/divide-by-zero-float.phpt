--TEST--
Division by zero test (float)
--FILE--
<?php

/**
 * @engine qb
 * @local float32 $\w+
 */
function test_function() {
	$a = 1;
	$b = $a / 0;
	echo $b;
}

test_function();

?>
--EXPECT--
INF
