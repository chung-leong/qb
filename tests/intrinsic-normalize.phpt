--TEST--
Vector normalization test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float32[4]		$a
 * @local	float32[3]		$b
 * @local	float32[2]		$c
 * @local	float64[5]		$d
 * @local	float64[*][4]	$e
 * @local	float64[*][4]	$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(0.5, 0.5, 0.5, 0.5);
	$b = array(1, 2, 3);
	$c = array(10, 5);
	$d = array(1, 2, 3, 4, 5);
	$e[] = array(1, -1, 1, 1);
	$e[] = array(2, -2, -2, 2);
	$e[] = array(1, 2, 3, 4);
	
	echo normalize($a), "\n";
	echo normalize($b), "\n";
	echo normalize($c), "\n";
	echo normalize($d), "\n";
	$f = normalize($e);

	foreach($f as $a) {
		echo $a, "\n";
	}
}

qb_compile();
ini_set('precision', 10);
test_function();

?>
--EXPECT--
[0.5, 0.5, 0.5, 0.5]
[0.26726, 0.53452, 0.80178]
[0.89443, 0.44721]
[0.1348399725, 0.269679945, 0.4045199175, 0.53935989, 0.6741998625]
[0.5, -0.5, 0.5, 0.5]
[0.5, -0.5, -0.5, 0.5]
[0.18257, 0.36515, 0.54772, 0.7303]
