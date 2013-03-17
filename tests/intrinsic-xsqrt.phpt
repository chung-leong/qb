--TEST--
Complex number square root test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]		$a
 * @local	float32[3][2]	$c
 *
 * @return	void
 *
 */
function test_function() {
	$a = array(2, 4);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = csqrt($c);
	echo csqrt($a), "\n";
	echo "$c\n";
}

qb_compile();

test_function();

?>
--EXPECT--
[1.798907, 1.111786]
[[2.498683, -1.800949], [0.1385727, 3.608213], [1.940506, -0.5153294]]
