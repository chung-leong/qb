--TEST--
Class variable resync test (magic method)
--FILE--
<?php

class ParentClass {

	private $a = 100;

	public function other_function() {
		echo "$this->a\n";
		$this->a = 102;
	}

	public function __get($name) {
		echo "__get($name) [$this->a]\n";
		if($name == 'a') {
			return $this->a;
		}
	}

	public function __set($name, $value) {
		echo "__set($name, $value) [$this->a]\n";
		if($name == 'a') {
			$this->a = $value;
		}
	}
}

/**
 * @property	int32	$a
 * 
 */
class TestClass extends ParentClass {
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
__get(a) [100]
100
__set(a, 101) [100]
101
__get(a) [102]
102
__set(a, 103) [102]
__get(a) [103]
103

