--TEST--
Vector increment test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[12]		$a1
 * @local	float32[10]		$b1
 * @local	float32[9]		$c1
 * @local	float32[13]		$d1
 *
 * @return	void
 *
 */
function test_function() {
	$a1++;
	$b1++;
	$c1++;
	$d1++;
	$d1--;
	echo "$a1 $b1 $c1 $d1\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1] [1, 1, 1, 1, 1, 1, 1, 1, 1, 1] [1, 1, 1, 1, 1, 1, 1, 1, 1] [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
