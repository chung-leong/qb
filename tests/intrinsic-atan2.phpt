--TEST--
Arctangent test (two parameters)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64	$[ab]
 * @param	float32	$[cd]
 * 
 * @return	void
 * 
 */
function test_function($a, $b, $c, $d) {
	echo atan2($a, 1), "\n";
	echo atan2($b, 2), "\n";
	echo atan2($c, 3), "\n";
	echo atan2($d, 4), "\n";
}

$a = 0;
$b = 1;
$c = 0.5;
$d = tan(M_PI / 2);
	
test_function($a, $b, $c, $d);

?>
--EXPECT--
0
0.46364760900081
0.1651487
1.570796

