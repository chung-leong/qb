--TEST--
Inline function argument element test
--FILE--
<?php

class TestClass {

	/** 
	 * @engine qb 
	 *
	 * @local float32[4]	$p
	 * @local float32[2][2] $q
	 */
	public function test1() {
		$p = array(1, 2, 3, 4);
		$q = array(5, 6, 7, 8);
		$this->test2($p, $q);
	}

	/** 
	 * @engine qb 
	 * @inline always
	 *
	 * @param float32[4]	$p
	 * @param float32[2][2] $q
	 */
	public function test2($p, $q) {
		echo "{$p[3]} {$q[1]}\n";
	}
}

$object = new TestClass;
$object->test1();

?>
--EXPECT--
4 [7, 8]
