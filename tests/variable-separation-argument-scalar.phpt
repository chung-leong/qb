--TEST--
Argument variable separation test (scalar)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32	$a
 * @param	uint32	$b
 * 
 * @return	void
 * 
 */
function test_function(&$a, $b) {
	$a = 4321;
	$b = 0;
}

qb_compile();

$var1 = 1234;
$var2 = $var1;

test_function($var1, $var2);

echo "$var1 $var2";

?>
--EXPECT--
4321 1234
