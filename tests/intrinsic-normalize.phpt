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

test_function();

?>
--EXPECT--
[0.5, 0.5, 0.5, 0.5]
[0.2672612, 0.5345225, 0.8017837]
[0.8944272, 0.4472136]
[0.13483997249265, 0.2696799449853, 0.40451991747795, 0.53935988997059, 0.67419986246324]
[0.5, -0.5, 0.5, 0.5]
[0.5, -0.5, -0.5, 0.5]
[0.1825742, 0.3651484, 0.5477226, 0.7302967]
