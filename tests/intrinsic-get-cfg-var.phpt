--TEST--
get_cfg_var test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @return	void
 * 
 */
function test_function() {
	if(get_cfg_var('chicken.donut')) {
		echo "Donut!\n";
	} else {
		echo "No donut!\n";
	}
}

test_function();

?>
--EXPECT--
No donut!
