--TEST--
Vector add test
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
	$a1 += $e1;
	$a1 += $a1;
	$b1 += $e1;
	$b1 += $b1;
	$c1 += $f1;
	$c1 += $c1;
	$d1 += $g1;
	$d1 += $d1;
	echo "$a1 $b1 $c1 $d1\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[2, 4, 6, 8, 2, 4, 6, 8, 2, 4, 6, 8] [2, 4, 6, 8, 2, 4, 6, 8, 2, 4] [10, 12, 14, 10, 12, 14, 10, 12, 14] [16, 18, 16, 18, 16, 18, 16, 18, 16, 18, 16, 18, 16]
