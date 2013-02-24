--TEST--
Class constant test (inheritance)
--FILE--
<?php

class ParentClass {
	const constant = 1234;
}

class TestClass extends ParentClass {

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

qb_compile();

$obj = new TestClass;
$obj->test();

?>
--EXPECT--
1234
1240
