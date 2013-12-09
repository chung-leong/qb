--TEST--
Inline function nested calls test
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
		$this->test2(0);
		$this->test2(8);
		$this->test2(2);
		echo "Function ends\n";
	}

	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32	$n
	 */
	public function test2($n) {
		if($n > 0) {
			$this->test3($n);
		} else {
			echo "$n is zero\n";
		}
	}
	
	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32	$n
	 */
	public function test3($n) {
		if($n % 2) {
			$this->test4($n);
		} else {
			$this->test5($n);
		}
	}
	
	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32	$n
	 */
	public function test4($n) {
		if($n > 5) {
			echo "$n is greater than 5\n";	
		} else {
			echo "$n is less than 5\n";
		}
	}
	
	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32	$n
	 */
	public function test5($n) {
		if($n > 4) {
			echo "$n is greater than 4\n";	
		} else {
			echo "$n is less than 4\n";
		}
	}
}

$object = new TestClass;
$object->test1();

?>
--EXPECT--
Function starts
1 is less than 5
6 is greater than 4
0 is zero
8 is greater than 4
2 is less than 4
Function ends
