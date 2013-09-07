--TEST--
HSV-to-RGB test
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
	$a = array(0, 0, 0.5);
	$b = array(0, 1, 1, 0.5);
	$c = array( array(300, 1, 1, 0.7), array(120, 1, 1, 0.4), array(240, 1, 1, 0.6) );
	
	echo hsv2rgb($a), "\n";
	echo hsv2rgb($b), "\n";
	echo hsv2rgb($c), "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[0.5, 0.5, 0.5]
[1, 0, 0, 0.5]
[[1, 0, 1, 0.7], [0, 1, 0, 0.4], [0, 0, 1, 0.6]]
