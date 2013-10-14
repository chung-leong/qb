--TEST--
Float as boolean test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	float64		$a
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = 0.0001;
	if($a) {
		echo "true\n";
	}
}

test_function();

?>
--EXPECT--
true
