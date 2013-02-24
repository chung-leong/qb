--TEST--
Natural log test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64	$[ab]
 * @local	float32	$[cd]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = M_E * M_E;
	$b = log($a);
	$c = M_E;
	$d = log($c);
	echo "$b $d\n";
}

qb_compile();

test_function();

?>
--EXPECT--
2 0.9999999
