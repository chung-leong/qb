--TEST--
Inline function nested if test
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
			if($n % 2) {
				if($n > 5) {
					echo "$n is greater than 5\n";	
				} else {
					echo "$n is less than 5\n";
				}
			} else {
				if($n > 4) {
					echo "$n is greater than 4\n";	
				} else {
					echo "$n is less than 4\n";
				}
			}
		} else {
			echo "$n is zero\n";
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
