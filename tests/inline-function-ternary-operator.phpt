--TEST--
Inline function ternary operator test
--FILE--
<?php

class TestClass {

	/** 
	 * @engine qb 
	 */
	public function test1() {
		echo "Function starts\n";
		$this->test2(1);
		$this->test2(6);
		echo "Function ends\n";
	}

	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32	$n
	 */
	public function test2($n) {
		echo ($n > 5) ? "$n is greater than 5\n" : "$n is less than 5\n";
	}
}

$object = new TestClass;
$object->test1();

?>
--EXPECT--
Function starts
1 is less than 5
6 is greater than 5
Function ends
