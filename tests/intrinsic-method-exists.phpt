--TEST--
Method existence test
--FILE--
<?php

class Test {
	/**
	 * A test function
	 * 
	 * @engine	qb
	 * 
	 * @return	void
	 * 
	 */
	function test_function() {
		if(method_exists($this, 'hello')) {
			echo "Function hello() exists!\n";
		}
		if(method_exists($this, 'hello2')) {
			echo "Function hello2() exists!\n";
		}
		if(method_exists('Test', 'hello')) {
			echo "Function hello() exists in class Test!\n";
		}
		if(method_exists(__CLASS__, 'hello')) {
			echo "Function hello() exists in class " . __CLASS__ . "!\n";
		}
	}
	
	function hello() {
	}
}

$obj = new Test;
$obj->test_function();

?>
--EXPECT--
Function hello() exists!
Function hello() exists in class Test!
Function hello() exists in class Test!
