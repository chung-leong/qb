--TEST--
Power function test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.6.0') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64		$[ab]
 * @local	float32		$[cd]
 * @local	int32		$e
 * @local	float64[2]	$f
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 2;
	$b = 8;
	$b = $a ** $b;
	$c = 4;
	$d = 10 ** $c;
	$e = 100;
	$a = $e ** 16;
	$f = [4, 2];
	$f **= -1;
	echo "$b $d $a $f\n";
}

test_function();

?>
--EXPECT--
256 10000 1.0E+32 [0.25, 0.5]
