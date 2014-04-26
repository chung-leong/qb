--TEST--
Vector element function class test 
--FILE--
<?php

class Test {
	/** @var vector */
	private $v;
	
	function __construct() {
		$this->v = array( 1, 2, 2 );
	}
							
	/** @engine qb */
	function test_function() {
		echo normalize($this->v)->y, "\n";
	}
}

$obj = new Test();
$obj->test_function();

?>
--EXPECTREGEX--
0.666666[67]
