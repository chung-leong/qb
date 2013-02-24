--TEST--
Static method call by value test (zend)
--FILE--
<?php 

class TestClass {

	public static function other_function($a, $b, $c) {
		print_r($a);
		print_r($b);
		print_r($c);
	}

	/** 
	 * @engine qb 
	 * @local int32 	$a
	 * @local int16[3]	$b
	 * @local string	$c
	 */
	public static function test() {
		$a = 1234;
		$b = array(99, 66, 77);
		$c = "something";
		self::other_function($a, $b, $c);
	}
}

qb_compile();

TestClass::test();

?>
--EXPECT--
1234Array
(
    [0] => 99
    [1] => 66
    [2] => 77
)
something