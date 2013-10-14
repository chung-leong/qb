--TEST--
RGB-to-HSV test
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
	$a = array(0.5, 0.5, 0.5);
	$b = array(1.0, 0.0, 0.0, 0.5);
	$c = array( array(1.0, 0.0, 1.0, 0.7), array(0.0, 1.0, 0.0, 0.4), array(0.0, 0.0, 1.0, 0.6) );
	
	echo rgb2hsv($a), "\n";
	echo rgb2hsv($b), "\n";
	echo rgb2hsv($c), "\n";
}

test_function();

?>
--EXPECT--
[0, 0, 0.5]
[0, 1, 1, 0.5]
[[300, 1, 1, 0.7], [120, 1, 1, 0.4], [240, 1, 1, 0.6]]
