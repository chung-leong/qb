--TEST--
Constant definition test
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
		define("GOOD", "Agnieszka");
	}
	if(defined('KASIA')) {
		define("BAD", "Kasia");
	}
	if(defined('BASIA')) {
		// not reachable
		define("UGLY", "Basia");
	}
}

qb_compile();

echo GOOD, "\n";
echo BAD, "\n";
echo @UGLY, "\n";

?>
--EXPECT--
Agnieszka
Kasia
UGLY
