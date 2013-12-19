--TEST--
Definition check test
--FILE--
<?php

define('AGNIESZKA', 10);
define('KASIA', 9);

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	if(defined('AGNIESZKA')) {
		echo "Agnieszka\n"; 
	}
	if(defined('KASIA')) {
		echo "Kasia\n";
	}
	if(defined('BASIA')) {
		echo "Basia\n";
	}
}

test_function();

?>
--EXPECT--
Agnieszka
Kasia
