--TEST--
Vector copy test
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
 * @local	float32[4]		$e1
 * @local	float32[3]		$f1
 * @local	float32[2]		$g1
 *
 * @return	void
 *
 */
function test_function() {
	$e1 = array(1, 2, 3, 4);
	$f1 = array(5, 6, 7);
	$g1 = array(8, 9);
	$a1 = $e1;
	$a1 = $a1;
	$b1 = $e1;
	$b1 = $f1;
	$b1 = $g1;
	$c1 = $e1;
	$c1 = $f1;
	$d1 = $g1;
	echo "$a1 $b1 $c1 $d1 $e1 $f1 $g1\n";
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4] [8, 9, 8, 9, 8, 9, 8, 9, 8, 9] [5, 6, 7, 5, 6, 7, 5, 6, 7] [8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8] [1, 2, 3, 4] [5, 6, 7] [8, 9]
