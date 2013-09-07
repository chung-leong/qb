--TEST--
RGB-to-HSL test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[3]			$a
 * @local	float32[4]			$b
 * @local	float32[3][4]		$c
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.25, 0.25, 0.25);
	$b = array(1.0, 0.2, 0.9, 0.5);
	$c = array( array(1.0, 0.0, 1.0, 0.7), array(0.0, 1.0, 0.0, 0.4), array(0.0, 0.0, 1.0, 0.6) );
	
	echo rgb2hsl($a), "\n";
	echo rgb2hsl($b), "\n";
	echo rgb2hsl($c), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0, 0, 0.25]
[307.5, 1, 0.6, 0.5]
[[300, 1, 0.5, 0.7], [120, 1, 0.5, 0.4], [240, 1, 0.5, 0.6]]
