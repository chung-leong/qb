--TEST--
Class variable test (scalar)
--FILE--
<?php

class TestClass {

	/** @var int32 */
	public $a = 1234;

	/** @var float64 */
	public $b = 1000;

	/** @var int8 */
	public $c = 6;

	/** @engine qb
	 *  @return	void */
	public function test() {
		echo "Inside qb: $this->a\n";
		$this->a = 5678;
		echo "Inside qb: $this->b\n";
		$this->b = 2000;
		echo "Inside qb: $this->c\n";
		$this->c = 129;
	}
}

$obj = new TestClass;

$obj->test();

echo "Outside qb: $obj->a\n";
echo "Outside qb: $obj->b\n";
echo "Outside qb: $obj->c\n";

?>
--EXPECT--
Inside qb: 1234
Inside qb: 1000
Inside qb: 6
Outside qb: 5678
Outside qb: 2000
Outside qb: -127

