--TEST--
Class variable resync test (array)
--FILE--
<?php 

class TestClass {

	/** @var int32[] */
	public $a = array(100, 2, 3);

	public function other_function() {
		print_r($this->a);
		$this->a = array(90, 33);
	}

	/** @engine qb */
	public function test() {
		echo "$this->a\n";
		$this->a = 123;
		$this->other_function();
		echo "$this->a\n";
		$this->a = array(1, 2, 3, 4);
	}
}

$obj = new TestClass;

$obj->test();
print_r($obj->a);

?>
--EXPECT--
[100, 2, 3]
Array
(
    [0] => 123
)
[90, 33]
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)

