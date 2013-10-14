--TEST--
HSL-to-RGB test
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
	$a = array(0, 0, 0.25);
	$b = array(307.5, 1, 0.6, 0.5);
	$c = array( array(300, 1, 0.5, 0.7), array(120, 1, 0.5, 0.4), array(240, 1, 0.5, 0.6) );
	
	echo hsl2rgb($a), "\n";
	echo hsl2rgb($b), "\n";
	echo hsl2rgb($c), "\n";
	echo hsl2rgb(rgb2hsl(array(0.25, 0.38, 0.78))), "\n";
}

test_function();

?>
--EXPECT--
[0.25, 0.25, 0.25]
[1, 0.2, 0.9, 0.5]
[[1, 0, 1, 0.7], [0, 1, 0, 0.4], [0, 0, 1, 0.6]]
[0.25, 0.38, 0.78]
