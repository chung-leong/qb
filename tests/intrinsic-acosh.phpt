--TEST--
Hyperbolic arcosine test
--SKIPIF--
<?php 
	if(!function_exists('acosh')) print 'skip acosh() not available';
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
	echo acosh($a), "\n";
	echo acosh($b), "\n";
	echo acosh($c), "\n";
	echo acosh($d), "\n";
}

qb_compile();

$a = 1;
$b = 2;
$c = 3;
$d = cosh(M_PI);
	
test_function($a, $b, $c, $d);

?>
--EXPECT--
0
1.3169578969248
1.762747
3.141593
