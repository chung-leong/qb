--TEST--
Complex number pow test
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
	
	$c = cpow($c, array(2, 0));
	echo cpow($a, array(3,-3)), "\n";
	echo "$c\n";
}

qb_compile();
ini_set('precision', 10);
test_function();

?>
--EXPECT--
[961.73, -2283.3]
[[-72, -54], [168, -26], [8.25, -14]]
