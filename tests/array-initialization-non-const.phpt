--TEST--
Array initialization test (non-constant expressions)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][][]	$a
 * @local	int32		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$b = 99;
	$a = array( 
		array(
			array(77, 1 + $b, $b, round(sin($b) * 10))
		),
	);
	echo $a;
}

qb_compile();

test_function();

?>
--EXPECT--
[77, 100, 99, -10]