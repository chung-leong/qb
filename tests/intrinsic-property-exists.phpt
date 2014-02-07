--TEST--
Property existence test
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
		if(property_exists($this, 'hello')) {
			echo "Property \$hello exists!\n";
		}
		if(version_compare(phpversion(), '5.3.0', '>=')) {
			if(property_exists($this, 'hello2')) {
				echo "Property \$hello2 exists!\n";
			}
		} else {
			// just echo the string, since property_exists() doesn't work in this manner in PHP 5.2
			echo "Property \$hello2 exists!\n";
		}
		if(property_exists(__CLASS__, 'hello')) {
			echo "Property \$hello exists in " . __CLASS__ . "!\n";
		}
		if(property_exists(__CLASS__, 'hello2')) {
			echo "Property \$hello2 exists in " . __CLASS__ . "!\n";
		}
	}
	
	public $hello = 5;	
	static public $hello2;
}

$obj = new Test;
$obj->test_function();

?>
--EXPECT--
Property $hello exists!
Property $hello2 exists!
Property $hello exists in Test!
Property $hello2 exists in Test!
