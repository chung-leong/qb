--TEST--
Inline function for loop test
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
	 * @local uint32	$i
	 */
	public function test2($n) {
		echo "Loop starts\n";
		for($i = 0; $i < $n; $i++) {
			echo "Iteration $i\n";
		}
		echo "Loop ends\n";
	}
}

$object = new TestClass;
$object->test1();

?>
--EXPECT--
Function starts
Loop starts
Iteration 0
Loop ends
Loop starts
Iteration 0
Iteration 1
Iteration 2
Iteration 3
Iteration 4
Iteration 5
Loop ends
Function ends
