--TEST--
Method call by reference test (zend)
--FILE--
<?php 

class TestClass {

	public function other_function(&$a, &$b, &$c) {
		$a = 33;
		$b = array(1, 2, 3);
		$c = "This is a test and this is only a test";
	}

	/**
	 * @engine qb
	 * @local int32		$a
	 * @local int16[3]	$b
	 * @local string	$c
	 */
	public function test() {
		$a = 1234;
		$b = array(99, 66, 77);
		$c = "something";
		$this->other_function($a, $b, $c);
		echo "$a $b $c\n";
	}
}

$obj = new TestClass;

$obj->test();

?>
--EXPECT--
33 [1, 2, 3] This is a test and this is only a test