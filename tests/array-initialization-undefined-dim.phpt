--TEST--
Array initialization test (undefined dimension)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[][]	$a
 * @local	int32[]		$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array( 
		array(1, 2, 3, 4, 5),
		array(2, 3, 4, 5, 6),
		array(3, 4, 5, 6, 7),
		array(4, 5, 6, 7)
	);
	
	foreach($a as $b) {
		echo "$b\n";
	}
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4, 5]
[2, 3, 4, 5, 6]
[3, 4, 5, 6, 7]
[4, 5, 6, 7, 0]
