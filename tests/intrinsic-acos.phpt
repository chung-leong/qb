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
--EXPECT--
1.5707963267949
0
1.0471975511966
1.5707963267949
---
1.5707963267949
0
1.047198
1.570796
