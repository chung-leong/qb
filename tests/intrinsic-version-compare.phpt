--TEST--
PHP version comparison test
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
	if(version_compare(phpversion(), "5.0", ">=")) {
		echo "Running in PHP 5.0 and higher\n";
	}
	echo version_compare("2.0", "3.0.1"), "\n";
	echo version_compare("3.1", "3.0.1"), "\n";
	echo version_compare("3.1.2", "3.1.2"), "\n";
	echo version_compare("3.1.2", "3.1.2", "!="), "\n";
}

test_function();

?>
--EXPECT--
Running in PHP 5.0 and higher
-1
1
0
0
