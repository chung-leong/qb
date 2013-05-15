--TEST--
Array initialization test (from object properties)
--FILE--
<?php

class TestClass {

	/** @var float32 */
	public $var1;

	/** @var float32 */
	public $var2;

	/** @var float32 */
	public $var3;

	/** @var float32 */
	public $var4;

	/**
	 * @engine	qb
	 * @local	float32[2][2]		$a
	 */
	function test() {
		$a = array(
			array(-$this->var1, $this->var2 >> 2),
			array($this->var3 / 100, $this->var4 * 0.01),
		);
		echo "$a\n";
	}
}

qb_compile();

$obj = new TestClass;
$obj->var1 = 100;
$obj->var2 = 200;
$obj->var3 = 300;
$obj->var4 = 400;
$obj->test();

?>
--EXPECT--
[[-100, 50], [3, 4]]
