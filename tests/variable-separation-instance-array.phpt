--TEST--
Object property variable separation test (array)
--FILE--
<?php

class test_class {

	/** @var int32[] */
	public $a;
	
	public $b;
	
	function __construct() {
		$this->a = array(1, 2, 3, 4);
		$this->b = $this->a;
	}

	/** @engine qb */
	function test() {
		$this->a = 4321;
	}
}

$object = new test_class;

qb_compile();

$object->test();

print_r($object->a);
print_r($object->b);

?>
--EXPECT--
Array
(
    [0] => 4321
    [1] => 4321
    [2] => 4321
    [3] => 4321
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)
