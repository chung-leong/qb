--TEST--
Tangent test
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
	echo tan($a), "\n";
	echo tan($b), "\n";
	echo tan($c), "\n";
	echo tan($d), "\n";
	echo tan($i), "\n";
}

qb_compile();

$a = M_PI / 4;
$b = M_PI / 2;
$c = 3;
$d = 2;
$i = 1;
	
echo tan($a), "\n";
echo tan($b), "\n";
echo tan($c), "\n";
echo tan($d), "\n";
echo tan($i), "\n";

echo "---\n";
test_function($a, $b, $c, $d, $i);

?>
--EXPECTREGEX--
1
1.633\d+E\+16
-0.14254654307428
-2.1850398632615
1.5574077246549
---
1
1.633\d+E\+16
-0.1425465
-2.18504
1.5574077246549
