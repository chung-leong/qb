--TEST--
Cross product test (4D analogue, variable dimension)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64[?][?]	$v1
 * @param	float64[?]		$v2
 * @param	float64[?]		$v3
 * @local	float64[?][?]	$r
 * 
 * @return	void
 * 
 */
function test_function($v1, $v2, $v3) {
	$r = cross($v1, $v2, $v3);
	echo "$r\n";
}

$v4_1 = array(array(1, 1, 1, 1), array(1, 2, 3, 4), array(0, 2, -4, -8), array(8, 5, 0, 0));
$v4_2 = array(1, -1, 0.5, 1);
$v4_3 = array(0.5, 0.5, 0.5, 0.5);

test_function($v4_1, $v4_2, $v4_3);

?>
--EXPECT--
[[0, 0, 0, 0], [-0.5, -0.75, 3, -1.75], [1.5, 2, -8, 4.5], [-1.25, 2, -8, 7.25]]
