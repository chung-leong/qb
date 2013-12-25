--TEST--
Class existence test
--FILE--
<?php

class A {

	/** @engine qb */
 	static function test() {
		echo "Class A exists!\n";
	}
}

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	if(class_exists('A')) {
		A::test();
	}
	if(class_exists('B')) {
		B::test();
	}
}

test_function();

?>
--EXPECT--
Class A exists!
