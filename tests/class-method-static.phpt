--TEST--
Trivial static class method test
--FILE--
<?php 

class TestClass {

	/** @engine qb */
	public static function test() {
		echo "Hello world";
	}
}

TestClass::test();

?>
--EXPECT--
Hello world