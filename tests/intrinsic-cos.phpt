--TEST--
Cosine test
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
	echo cos($a), "\n";
	echo cos($b), "\n";
	echo cos($c), "\n";
	echo cos($d), "\n";
	echo cos($i), "\n";
}

qb_compile();

$a = M_PI / 4;
$b = M_PI / 2;
$c = 3;
$d = 2;
$i = 1;
	
ini_set('precision', 10);

test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
0.7071067812
6.123233996E-17
-0.98999
-0.41615
0.5403023059
