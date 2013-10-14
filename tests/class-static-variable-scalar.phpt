--TEST--
Class static variable test (scalar)
--FILE--
<?php

class TestClass {

	/** @var int32 */
	public static $a = 1234;

	/** @engine qb */
	public function test() {
		echo "Inside qb: ", self::$a, "\n";
		self::$a = 5678;
	}
}

$obj = new TestClass;

$obj->test();

echo "Outside qb: ", TestClass::$a, "\n";

?>
--EXPECT--
Inside qb: 1234
Outside qb: 5678
