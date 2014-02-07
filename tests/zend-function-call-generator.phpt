--TEST--
Zend generator call
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5.0') < 0) print 'skip feature not available';
?>
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
	generator(5);
}
	
function generator($count) {
	for($i = 0; $i < $count; $i++) {
		yield $i;
	}
}

test_function();

?>
--EXPECTREGEX--
.*Fatal error.*
