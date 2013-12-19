--TEST--
Trivial class method test
--FILE--
<?php

class TestClass {

	/** @engine qb */
 	public function test() {
		echo "Hello world";
	}
}

$obj = new TestClass;

$obj->test();

?>
--EXPECT--
Hello world