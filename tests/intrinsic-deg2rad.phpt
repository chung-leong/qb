--TEST--
Degree to radian test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32	$a
 * @local	int16	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 45;
	$b = 18000;
	echo deg2rad(360), "\n";
	echo deg2rad(90), "\n";
	echo deg2rad(720), "\n";
	echo deg2rad($a), "\n";
	echo deg2rad($b), "\n";
}

test_function();

?>
--EXPECT--
6.2831853071796
1.5707963267949
12.566370614359
0.78539816339745
314.15926535898