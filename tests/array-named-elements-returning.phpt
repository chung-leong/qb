--TEST--
Array with named elements returning object test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[x, y]	$a
 * 
 * @return	float32[x, y]
 * 
 */
function test_function() {
	$a->x = 123;
	$a->y = 456;
	return $a;
}

qb_compile();

$point = test_function();

print_r($point);

?>
--EXPECT--
stdClass Object
(
    [x] => 123
    [y] => 456
)