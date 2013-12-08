--TEST--
Inline function class variable test
--SKIPIF--
<?php

class TestClass {

	/** @var float32 */
	public $number = 14.5;

	/** @engine qb */
	public function test1() {
		$this->test2();
	}

	/** @engine qb */
	public function test2() {
		echo "Value of number is $this->number\n";
	}
}

$object = new TestClass;
$object->test1();
$object->test2();

?>