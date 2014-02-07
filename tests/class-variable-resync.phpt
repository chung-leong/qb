--TEST--
Class variable resync test
--FILE--
<?php

/**
 * @property	int32	$a
 * 
 */
class TestClass {

	public $a = 100;

	public function other_function() {
		echo "$this->a\n";
		$this->a = 102;
	}

	/** @engine qb */
	public function test() {
		echo "$this->a\n";
		$this->a = 101;
		$this->other_function();
		echo "$this->a\n";
		$this->a = 103;
	}
}

$obj = new TestClass;

$obj->test();
echo "$obj->a\n";

?>
--EXPECT--
100
101
102
103
