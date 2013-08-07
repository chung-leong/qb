--TEST--
Complex number sine test
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
	
	$c = csin($c);
	echo csin($a), "\n";
	echo "$c\n";
}

qb_compile();
ini_set('precision', 10);
test_function();

?>
--EXPECT--
[24.831, -11.357]
[[571.75, 4011], [-0.64835, 1.0664], [-1.3197, 3.3964]]
