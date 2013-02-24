--TEST--
Radian to degree test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$a
 * @local	int16	$b
 * @local	float32	$c
 * @local	float32	$pi1
 * @local	float64	$pi2
 * 
 * @return	void
 * 
 */
function test_function() {
	$pi1 = M_PI;
	$a = rad2deg($pi1);
	$b = rad2deg($pi1);
	$c = rad2deg($pi1);
	echo "$a, $b, $c\n";

	$pi2 = M_PI;
	$a = rad2deg($pi2);
	$b = rad2deg($pi2);
	$c = rad2deg($pi2);
	echo "$a, $b, $c\n";
	
	$a = rad2deg(2);
	$b = rad2deg(2);
	$c = rad2deg(2);	
	echo "$a, $b, $c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
180, 180, 180
180, 180, 180
114, 114, 114.5916
