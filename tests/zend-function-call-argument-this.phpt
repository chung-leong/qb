--TEST--
Zend function call with $this
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
		dump($this);
	}
	
	public $hello = 5;	
}

$obj = new Test;
$obj->test_function();

function dump($object) {
	print_r($object);
}

?>
--EXPECT--
Test Object
(
    [hello] => 5
)
