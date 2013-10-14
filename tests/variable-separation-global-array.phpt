--TEST--
Global variable separation test (array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @global	int32[]	$a
 * 
 * @return	void
 * 
 */
function test_function() {
	global $a;
	$a = 4321;
}

$a = array(1, 2, 3, 4);
$b = $a;

test_function();

print_r($a);
print_r($b);

?>
--EXPECT--
Array
(
    [0] => 4321
    [1] => 4321
    [2] => 4321
    [3] => 4321
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)
