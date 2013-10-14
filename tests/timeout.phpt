--TEST--
Timeout test
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
set_time_limit(2);

function test_function() {
	while(1) {
	}
}

test_function();

?>
--EXPECTREGEX--
Fatal error: Maximum execution time of 2 seconds exceeded in .+ on line \d+
