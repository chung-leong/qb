--TEST--
Class variable resync test (magic method, array)
--FILE--
<?php

class ParentClass {

	private $a = array(100, 2, 3);

	public function other_function() {
		echo "[" . implode(', ', $this->a) . "]\n";
		$this->a = array(90, 33);
	}

	public function __get($name) {
		echo "__get($name) [" . implode(', ', $this->a) . "]\n";
		if($name == 'a') {
			return $this->a;
		}
	}

	public function __set($name, $value) {
		echo "__set($name, [" . implode(', ', $value) . "]) [" . implode(', ', $this->a) . "]\n";
		if($name == 'a') {
			$this->a = $value;
		}
	}
}

/**
 * @property	int32[]	$a
 * 
 */
class TestClass extends ParentClass {
	/** @engine qb */
	public function test() {
		echo "$this->a\n";
		$this->a = array(1, 2, 3, 4);
		$this->other_function();
		echo "$this->a\n";
		$this->a = array();
	}
}

$obj = new TestClass;

$obj->test();
print_r($obj->a);

?>
--EXPECT--
__get(a) [100, 2, 3]
[100, 2, 3]
__set(a, [1, 2, 3, 4]) [100, 2, 3]
[1, 2, 3, 4]
__get(a) [90, 33]
[90, 33]
__set(a, []) [90, 33]
__get(a) []
Array
(
)

