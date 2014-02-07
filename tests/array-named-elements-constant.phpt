--TEST--
Array with named elements test (constant)
--FILE--
<?php

define("RGBA", "r,g,b,a");

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[RGBA]	$a
 * 
 * @return	void
 * 
 */
function test_function($a) {
	echo "$a->r $a->g $a->b $a->a";
}

$object = new stdClass;
$object->r = 1;
$object->g = 0.2;
$object->b = 0.5;

test_function($object);

?>
--EXPECT--
1 0.2 0.5 0