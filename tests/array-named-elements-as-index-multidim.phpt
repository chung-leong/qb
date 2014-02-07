--TEST--
Array with named elements as index test (multidimensional)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[2][4]	$a
 * @local	float32			$b
 * @local	int32[x,y]	$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[$c->x][$c->y] = 5;
	$b = $a[$c->x][$c->y];
	unset($a[$c->x][$c->y]);
	isset($a[$c->x][$c->y]);
	echo "$a $b\n";
}

test_function();

?>
--EXPECT--
[[0, 0, 0, 0], [0, 0, 0, 0]] 5
