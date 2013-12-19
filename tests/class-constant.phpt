--TEST--
Class constant test
--FILE--
<?php

class TestClass {
	const constant = 1234;

	/** 
	 * @engine qb
	 * @local int32 $.*
	 */
	public function test() {
		$a = self::constant + 6;
		echo self::constant, "\n";
		echo $a, "\n";
	}
}

$obj = new TestClass;
$obj->test();

?>
--EXPECT--
1234
1240
