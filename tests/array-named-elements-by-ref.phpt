--TEST--
Array with named elements by ref test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[r,g,b,a]	$a
 * 
 * @return	void
 * 
 */
function test_function(&$a) {
	$a->r /= 2;
	$a->g /= 2;
	$a->b /= 2;
	$a->a = 1;
}

$object = new stdClass;
$object->r = 1;
$object->g = 0.25;
$object->b = 0.5;

test_function($object);

print_r($object);

?>
--EXPECT--
stdClass Object
(
    [r] => 0.5
    [g] => 0.125
    [b] => 0.25
    [a] => 1
)
