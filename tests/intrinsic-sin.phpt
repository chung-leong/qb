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
	
echo sin($a), "\n";
echo sin($b), "\n";
echo sin($c), "\n";
echo sin($d), "\n";
echo sin($i), "\n";

echo "---\n";
test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
0.70710678118655
1
0.14112000805987
0.90929742682568
0.8414709848079
---
0.70710678118655
1
0.14112
0.9092974
0.8414709848079
