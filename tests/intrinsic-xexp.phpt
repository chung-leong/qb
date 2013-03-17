--TEST--
Complex number ecponential test
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

qb_compile();

test_function();

?>
--EXPECT--
[-4.829809, -5.592056]
[[-18.30054, -8.277621], [1.221261E-6, 1.902002E-6], [-13.78089, -30.1118]]
