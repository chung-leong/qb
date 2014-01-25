--TEST--
ini_set test
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
	ini_set('qb.column_major_matrix', true);

	if(ini_get('qb.column_major_matrix')) {
		echo "qb.column_major_matrix = on\n";
	} else {
		echo "qb.column_major_matrix = off\n";
	}

	ini_restore('qb.column_major_matrix');
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
qb.column_major_matrix = off

