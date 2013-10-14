--TEST--
Chained assignment test (array)
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	int32[4]		$.*
 *
 * @return	void
 *
 */
function test_function() {
	$a = $b = $c = $d = 88;
	echo "$a $b $c $d\n";
}

test_function();

?>
--EXPECT--
[88, 88, 88, 88] [88, 88, 88, 88] [88, 88, 88, 88] [88, 88, 88, 88]
