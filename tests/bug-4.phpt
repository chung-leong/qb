--TEST--
Bug #4
--FILE--
<?php

/**
 * @engine qb
 * @local float32[2][2]	$matrix 
 * @local float32		$(rotation|cos|sin)
 */
function test_function() {
	$rotation = deg2rad(0);
	$cos = cos($rotation);
	$sin = sin($rotation);
	$matrix = array( array(1, 0), array(0, 1) );
	$matrix = mm_mult($matrix, array( array($cos, -$sin), array($sin,  $cos)) );
	echo $matrix;
}

test_function();

?>
--EXPECT--
[[1, 0], [0, 1]]
