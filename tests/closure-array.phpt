--TEST--
Closure test (array)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3.0') < 0) print 'skip feature not available';
?>
--FILE--
<?php

function outer_function() {
	$a = array(1, 2, 3, 4);

	/**
	 * A test function
	 * 
	 * @engine	qb
	 * @lexical	int32[]	$a
	 * 
	 * @return	void
	 * 
	 */
	$f = function() use(&$a) {
		echo "Inside qb: $a\n";
		$a = array(4, 5, 6, 7);
	};

	$f();
	echo "Outside qb:\n";
	print_r($a);
}

outer_function();

?>
--EXPECT--
Inside qb: [1, 2, 3, 4]
Outside qb:
Array
(
    [0] => 4
    [1] => 5
    [2] => 6
    [3] => 7
)

