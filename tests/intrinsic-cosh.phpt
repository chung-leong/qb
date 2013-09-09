--TEST--
Hyperbolic cosine test
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
	echo cosh($a), "\n";
	echo cosh($b), "\n";
	echo cosh($c), "\n";
	echo cosh($d), "\n";
	echo cosh($i), "\n";
}

qb_compile();

$a = M_PI / 4;
$b = M_PI / 2;
$c = 3;
$d = 2;
$i = 1;
	
test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
1.324609089252
2.5091784786581
10.06766
3.762196
1.5430806348152
