--TEST--
Argument by ref test (array)
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @param	int32[4]	$a
 * @param	float64[2]	$b
 *
 * @return	void
 *
 */
function test_function(&$a, &$b) {
	$a[0] = 2;
	$a[1] = 4;
	$a[2] = 6;
	$a[3] = 8;
	$b = array(3.14, 99.99);
}

$a = array(1, 2, 3, 4);
$b = array();

test_function($a, $b);

print_r($a);
print_r($b);

?>
--EXPECT--
Array
(
    [0] => 2
    [1] => 4
    [2] => 6
    [3] => 8
)
Array
(
    [0] => 3.14
    [1] => 99.99
)
