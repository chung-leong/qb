--TEST--
ini_get test
--FILE--
<?php

ini_set('qb.column_major_matrix', true);

/**
 * A test function
 * 
 * @engine	qb
 * @return	void
 * 
 */
function test_function() {
	if(ini_get('qb.column_major_matrix')) {
		echo "qb.column_major_matrix = on\n";
	} else {
		echo "qb.column_major_matrix = off\n";
	}
}

test_function();

?>
--EXPECT--
qb.column_major_matrix = on

