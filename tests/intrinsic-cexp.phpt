--TEST--
Complex number exponential test
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
	
	$c = cexp($c);
	echo cexp($a), "\n";
	echo "$c\n";
}

ini_set('precision', 10);
test_function();

?>
--EXPECT--
[-4.8298, -5.5921]
[[-18.301, -8.2776], [1.2213E-6, 1.902E-6], [-13.781, -30.112]]
