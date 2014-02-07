--TEST--
Object property variable separation test (array)
--FILE--
<?php 

class test_class {

	/** @var int32[] */
	public static $a;
	
	public static $b;
	
	/** @engine qb */
	function test() {
		self::$a = 4321;
	}
}

test_class::$a = array(1, 2, 3, 4);
test_class::$b = test_class::$a;

$object = new test_class;

$object->test();

print_r(test_class::$a);
print_r(test_class::$b);

?>
--EXPECT--
Array
(
    [0] => 4321
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)
