--TEST--
Arcosine test
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
	echo acos($a), "\n";
	echo acos($b), "\n";
	echo acos($c), "\n";
	echo acos($d), "\n";
}

$a = 0;
$b = 1;
$c = 0.5;
$d = cos(M_PI / 2);
	
ini_set('precision', 10);

test_function($a, $b, $c, $d);

?>
--EXPECT--
1.570796327
0
1.0472
1.5708
