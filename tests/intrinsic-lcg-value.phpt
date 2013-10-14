--TEST--
Random number test (lcg_value)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32	$a
 * @local	float64	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = lcg_value();
	$b = lcg_value();
	
	if($a >= 0 && $a <= 1) {
		echo "a in range\n";
	}
	if($b >= 0 && $b <= 1) {
		echo "b in range\n";
	}
}

test_function();

?>
--EXPECT--
a in range
b in range
