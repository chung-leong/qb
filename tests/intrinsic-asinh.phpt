--TEST--
Hyperbolic arcsine test
--SKIPIF--
<?php 
	if(!function_exists('asinh')) print 'skip asinh() not available';
?>
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
	echo asinh($a), "\n";
	echo asinh($b), "\n";
	echo asinh($c), "\n";
	echo asinh($d), "\n";
}

qb_compile();

$a = 1;
$b = 2;
$c = 3;
$d = sinh(M_PI);
	
ini_set('precision', 10);

test_function($a, $b, $c, $d);

?>
--EXPECT--
0.881373587
1.443635475
1.8184
3.1416
