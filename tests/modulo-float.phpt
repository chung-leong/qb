--TEST--
Floating point modulo test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64		$a
 * @local	float64		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 3.8;
	$b = $a % 2;
	echo "$a % 2 = $b\n";

	$a = -3.8;
	$b = $a % 2;
	echo "$a % 2 = $b\n";

	$a = -3.8;
	$b = fmod($a, 2);
	echo "fmod($a, 2) = $b\n";
	
	$a = -3.8;
	$b = mod($a, 2);
	echo "mod($a, 2) = $b\n";
}

qb_compile();

test_function();

?>
--EXPECT--
3.8 % 2 = 1.8
-3.8 % 2 = -1.8
fmod(-3.8, 2) = -1.8
mod(-3.8, 2) = 0.2
