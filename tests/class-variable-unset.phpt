--TEST--
Unset test (class variable)
--FILE--
<?php

/**
 * @property	uint8[*]	$a
 * 
 */
class TestClass {

	public $a = "Hello";

	/** @engine qb */
	public function test() {
		echo "$this->a\n";
		unset($this->a);
		echo "$this->a\n";
	}
}

qb_compile();

$obj = new TestClass;
$obj->test();
echo "$obj->a\n";

?>
--EXPECT--
[72, 101, 108, 108, 111]
[]

