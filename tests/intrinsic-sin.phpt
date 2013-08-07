--TEST--
Sine test
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
	echo sin($a), "\n";
	echo sin($b), "\n";
	echo sin($c), "\n";
	echo sin($d), "\n";
	echo sin($i), "\n";
}

qb_compile();

$a = M_PI / 4;
$b = M_PI / 2;
$c = 3;
$d = 2;
$i = 1;
	
ini_set('precision', 10);

echo sin($a), "\n";
echo sin($b), "\n";
echo sin($c), "\n";
echo sin($d), "\n";
echo sin($i), "\n";

echo "---\n";
test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
0.7071067812
1
0.1411200081
0.9092974268
0.8414709848
---
0.7071067812
1
0.14112
0.9093
0.8414709848
