--TEST--
Class variable increment test (scalar)
--FILE--
<?php 

class TestClass {

	/** @var int32 */
	public $a = 1234;

	/** @engine qb */
	public function test() {
		echo "Inside qb: $this->a\n";
		$this->a++;
		++$this->a;
		$this->a--;
		$this->a += 5;
	}
}

$obj = new TestClass;

$obj->test();

echo "Outside qb: $obj->a\n";

?>
--EXPECT--
Inside qb: 1234
Outside qb: 1240
