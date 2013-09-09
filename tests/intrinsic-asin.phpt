--TEST--
Arcsine test
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
	echo asin($a), "\n";
	echo asin($b), "\n";
	echo asin($c), "\n";
	echo asin($d), "\n";
}

qb_compile();

$a = 0;
$b = 1;
$c = 0.5;
$d = sin(M_PI / 2);
	
test_function($a, $b, $c, $d);

?>
--EXPECT--
0
1.5707963267949
0.5235988
1.570796
