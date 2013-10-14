--TEST--
Chained assignment test (scalar)
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	int32		$.*
 *
 * @return	void
 *
 */
function test_function() {
	$b = 1;
	$a = $b = $c = $d = $b * 88;	// assignment to $d will be elimited, as it'll be the write target of $b * 88
	echo "$a $b $c $d\n";
}

test_function();

?>
--EXPECT--
88 88 88 88
