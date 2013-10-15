--TEST--
Argument variable separation test (array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int32[]		$a
 * @param	uint32[]	$b
 * 
 * @return	void
 * 
 */
function test_function(&$a, $b) {
	$a = 4321;
	$b = 0;
}

$var1 = array(1, 2, 3, 4);
$var2 = $var1;

test_function($var1, $var2);

print_r($var1);
print_r($var2);

?>
--EXPECT--
Array
(
    [0] => 4321
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)
