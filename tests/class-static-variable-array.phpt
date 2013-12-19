--TEST--
Class static variable test (array)
--FILE--
<?php 

class TestClass {

	/** @var int32[4] */
	public static $a = array(1, 2, 3, 4);

	/** @engine qb */
	public function test() {
		echo "Inside qb: ", self::$a, "\n";
		self::$a *= 2;
	}
}

$obj = new TestClass;

$obj->test();

echo "Outside qb: [", implode(TestClass::$a, ', '), "]\n";

?>
--EXPECT--
Inside qb: [1, 2, 3, 4]
Outside qb: [2, 4, 6, 8]
