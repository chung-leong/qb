--TEST--
Class variable test (scalar)
--FILE--
<?php

class TestClass {

	/** @var int32 */
	public $a = 1234;

	/** @engine qb
	 *  @return	void */
	public function test() {
		echo "Inside qb: $this->a\n";
		$this->a = 5678;
	}
}

$obj = new TestClass;

qb_compile();

$obj->test();

echo "Outside qb: $obj->a\n";

?>
--EXPECT--
Inside qb: 1234
Outside qb: 5678
