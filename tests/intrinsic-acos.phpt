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

qb_compile();

$a = 0;
$b = 1;
$c = 0.5;
$d = cos(M_PI / 2);
	
echo acos($a), "\n";
echo acos($b), "\n";
echo acos($c), "\n";
echo acos($d), "\n";

echo "---\n";
test_function($a, $b, $c, $d);

?>
--EXPECTREGEX--
1\.570796326794\d
0
1\.047197551196\d
1\.570796326794\d
---
1\.570796326794\d
0
1\.04719\d
1\.57079\d
