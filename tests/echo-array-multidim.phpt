--TEST--
Echo variable test (multidimensional array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*][2][4]	$a
 * @local	float64[4][4]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a[] = array( array(1, 2, 3, 4), array(5, 6, 7, 8));
	$a[] = array( array(10, 20, 30, 40), array(50, 60, 70, 80));
	$a[] = array( array(100, 200, 300, 400), array(500, 600, 700, 800));
	
	$b = array(
		array(0.299,  0.596,  0.212, 0.000),
		array(0.587, -0.275, -0.523, 0.000),
		array(0.114, -0.321,  0.311, 0.000),
		array(0.000,  0.000,  0.000, 1.000),
	);
	
	echo $a, "\n";
	echo $b, "\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[[[1, 2, 3, 4], [5, 6, 7, 8]], [[10, 20, 30, 40], [50, 60, 70, 80]], [[100, 200, 300, 400], [500, 600, 700, 800]]]
[[0.299, 0.596, 0.212, 0], [0.587, -0.275, -0.523, 0], [0.114, -0.321, 0.311, 0], [0, 0, 0, 1]]
