--TEST--
Inline function local array test
--FILE--
<?php

class TestClass {

	/** 
	 * @engine qb 
	 */
	public function test1() {
		$this->test2();
	}

	/** 
	 * @engine qb 
	 *
	 * @local float32[3]	$a
	 * @local float32[3][3] $b
	 */
	public function test2() {
		echo "$a $b\n";
	}
}

$object = new TestClass;
$object->test1();

?>
--EXPECT--
[0, 0, 0] [[0, 0, 0], [0, 0, 0], [0, 0, 0]]
