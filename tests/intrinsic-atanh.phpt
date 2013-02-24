--TEST--
Hyperbolic arctangent test
--SKIPIF--
<?php 
	if(!function_exists('atanh')) print 'skip atanh() not available';
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
	echo atanh($a), "\n";
	echo atanh($b), "\n";
	echo atanh($c), "\n";
	echo atanh($d), "\n";
}

qb_compile();

$a = 1;
$b = 2;
$c = 3;
$d = tanh(M_PI);
	
echo atanh($a), "\n";
echo atanh($b), "\n";
echo atanh($c), "\n";
echo atanh($d), "\n";

echo "---\n";
test_function($a, $b, $c, $d);

?>
--EXPECT--
INF
NAN
NAN
3.1415926535898
---
INF
NAN
NAN
3.141594
