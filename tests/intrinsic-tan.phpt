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

ini_set('precision', 10);
	
test_function($a, $b, $c, $d, $i);

?>
--EXPECT--
1
1.633123935E+16
-0.14255
-2.185
1.557407725

