--TEST--
Hyperbolic arcsine test
--SKIPIF--
<?php 
	if(!function_exists('asinh')) print 'skip asinh() not available';
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
	echo asinh($a), "\n";
	echo asinh($b), "\n";
	echo asinh($c), "\n";
	echo asinh($d), "\n";
}

qb_compile();

$a = 1;
$b = 2;
$c = 3;
$d = sinh(M_PI);
	
echo asinh($a), "\n";
echo asinh($b), "\n";
echo asinh($c), "\n";
echo asinh($d), "\n";

echo "---\n";
test_function($a, $b, $c, $d);

?>
--EXPECTREGEX--
0\.88137358701954
1\.4436354751788
1\.8184464592321
3\.1415926535898
---
0\.88137358701954
1\.4436354751788
1\.81844[67]
3\.141593
