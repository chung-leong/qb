--TEST--
Method call by value test (zend)
--FILE--
<?php 

class TestClass {

	public function other_function($a, $b, $c) {
		print_r($a);
		print_r($b);
		print_r($c);
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

qb_compile();

$obj = new TestClass;
$obj->test();

?>
--EXPECT--
1234Array
(
    [0] => 99
    [1] => 66
    [2] => 77
)
something