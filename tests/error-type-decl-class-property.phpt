--TEST--
Error reporting test (unexpected keyword in type declaration for class property)
--FILE--
<?php

class TestClass { 

   	/** @engine qb */
	
	
	public $var;

	/** @engine qb */
	function test_function() {
		echo "Hello\n";
	}
}

$obj = new TestClass;
$obj->test_function();

?>
--EXPECTREGEX--
.*line 5.*