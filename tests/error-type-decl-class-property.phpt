--TEST--
Error reporting test (unexpected keyword in type declaration for class property)
--FILE--
<?php

class TestClass { 

	/** @engine qb */
	public $var;

	/** @engine	qb */
	function test_function() {
	}
}

qb_compile();

?>
--EXPECTREGEX--
.*line 5.*