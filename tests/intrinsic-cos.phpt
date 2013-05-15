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
	
echo cos($a), "\n";
echo cos($b), "\n";
echo cos($c), "\n";
echo cos($d), "\n";
echo cos($i), "\n";

echo "---\n";
test_function($a, $b, $c, $d, $i);

?>
--EXPECTREGEX--
0.70710678118655
6.123\d+E\-0?17
-0.98999249660045
-0.41614683654714
0.54030230586814
---
0.70710678118655
6.123\d+E\-?17
-0.9899925
-0.4161468
0.54030230586814
