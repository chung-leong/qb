--TEST--
Complex number multiplication test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]		$a
 * @local	float32[2]		$b
 * @local	float32[3][2]	$c
 * @local	complex			$e
 * @local	complex			$f
 *
 * @return	void
 *
 */
function test_function() {
	$a = $e = array(2, 4);
	$b = $f = array(8, -7);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = cmult($c, $b);
	echo $a * $b, "\n";
	echo cmult($a, $b), "\n";
	echo $e * $f, "\n";	
	echo "$c\n";
}

test_function();

?>
--EXPECT--
[16, -28]
[44, 18]
[44, 18]
[[-39, -93], [-97, 99], [14, -40.5]]
