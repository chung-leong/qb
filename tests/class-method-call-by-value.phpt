--TEST--
Method call by value test
--FILE--
<?php 

class TestClass {

	/**
	 * @engine qb
	 * @param int32		$a
	 * @param int16[]	$b
	 * @param string	$c
	 */
	public function other_function($a, $b, $c) {
		echo "$a, $b, $c";
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
	}
}

$obj = new TestClass;
$obj->test();

?>
--EXPECT--
1234, [99, 66, 77], something
