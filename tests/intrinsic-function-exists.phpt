--TEST--
Function existence test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	if(function_exists('test_function')) {
		echo "This function exists!\n";
	}
	if(function_exists('test_function2')) {
		echo "This other function exists too!\n";
	}
}

function test_function2() {
}

test_function();

?>
--EXPECT--
This function exists!
This other function exists too!
