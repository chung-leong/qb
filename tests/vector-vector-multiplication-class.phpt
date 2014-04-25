--TEST--
Vector-vector multiplication test (inside class)
--FILE--
<?php

class Test {
	/** @var vector */
	private $v1;
	
	/** @var vector */
	private $v2;
	
	function __construct() {
		$this->v1 = array( 1, 2, 3 );
		$this->v2 = array( 4, 5, 6 );
	}
							
	/** @engine qb */
	function test_function() {
		echo $this->v1 * $this->v2, "\n";
	}
}

$obj = new Test();
$obj->test_function();

?>
--EXPECT--
32
