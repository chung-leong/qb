--TEST--
Argument by ref test (multidimensional array)
--FILE--
<?php 

class TestClass {

	/**
	 * A test function
	 *
	 * @engine	qb 
	 * @param	int32[4][4]	$a
	 *
	 * @return	void
	 *
	 */
	function test(&$a) {
		$a[0] = 2;
		$a[1] = 4;
		$a[2] = 6;
		$a[3] = 8;
		echo "$a\n";
	}
}

$a = array();

$obj = new TestClass;
$obj->test($a);

print_r($a);

?>
--EXPECT--
[[2, 2, 2, 2], [4, 4, 4, 4], [6, 6, 6, 6], [8, 8, 8, 8]]
Array
(
    [0] => Array
        (
            [0] => 2
            [1] => 2
            [2] => 2
            [3] => 2
        )

    [1] => Array
        (
            [0] => 4
            [1] => 4
            [2] => 4
            [3] => 4
        )

    [2] => Array
        (
            [0] => 6
            [1] => 6
            [2] => 6
            [3] => 6
        )

    [3] => Array
        (
            [0] => 8
            [1] => 8
            [2] => 8
            [3] => 8
        )

)
