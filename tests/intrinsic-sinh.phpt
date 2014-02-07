--TEST--
Hyperbolic sine test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float64	$[ab]
 * @param	float32	$[cd]
 * @param	int32	$i
 * 
 * @return	void
 * 
 */
function test_function($a, $b, $c, $d, $i) {
	echo sinh($a), "\n";
	echo sinh($b), "\n";
	echo sinh($c), "\n";
	echo sinh($d), "\n";
	echo sinh($i), "\n";
}

$a = M_PI / 4;
$b = M_PI / 2;
$c = 3;
$d = 2;
$i = 1;
	
test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
0.86867096148601
2.3012989023073
10.01787
3.62686
1.1752011936438
