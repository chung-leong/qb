--TEST--
Chained assignment test (element)
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	int32[4]		$a
 * @local	int32			$b
 *
 * @return	void
 *
 */
function test_function() {
	$b = 1;
	$a[0] = $a[1] = $a[2] = $b * 88;
	echo "$a\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[88, 88, 88, 0]
