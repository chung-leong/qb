--TEST--
Array with named elements test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[r, g, b, a]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "$a->r $a->g $a->b $a->a";
}

qb_compile();

$object = new stdClass;
$object->r = 1;
$object->g = 0.2;
$object->b = 0.5;

test_function($object);

?>
--EXPECT--
1 0.2 0.5 0