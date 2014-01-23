--TEST--
Closure test (scalar)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3.0') < 0) print 'skip feature not available';
?>
--FILE--
<?php

function outer_function() {
	$a = 1968;

	/**
	 * A test function
	 * 
	 * @engine	qb
	 * @lexical	int32	$a
	 * 
	 * @return	void
	 * 
	 */
	$f = function() use(&$a) {
		echo "Inside qb: $a\n";
		$a = 2021;
	};

	$f();
	echo "Outside qb: $a\n";
}

outer_function();

?>
--EXPECT--
Inside qb: 1968
Outside qb: 2021
