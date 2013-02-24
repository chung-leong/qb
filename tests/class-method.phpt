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

qb_compile();

$obj->test();

?>
--EXPECT--
Hello world