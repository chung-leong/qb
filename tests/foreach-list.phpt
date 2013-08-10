--TEST--
Foreach loop with list test
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
 * @local	uint32[2][3][2]	$array1
 * @local	uint32[2]		$(a|b|c)
 * @local	uint32[2][2]	$array2
 * @local	uint32			$(e|f|g)
 * 
 * @return	void
 * 
 */
function test_function() {
	$array1 = [
	    [[1, 10], [2, 20], [3, 30]],
	    [[4, 40], [5, 50], [6, 60]],
	];
	foreach ($array1 as list($a, $b, $c)) {
	    echo "A: $a; B: $b; C: $c\n";
	}
	
	$array2 = [
	    [1, 2],
	    [3, 4],
	];
	foreach ($array2 as list($e, $f)) {
	    echo "E: $e; F: $f\n";
	}
}

qb_compile();

test_function();

?>
--EXPECT--
A: [1, 10]; B: [2, 20]; C: [3, 30]
A: [4, 40]; B: [5, 50]; C: [6, 60]
E: 1; F: 2
E: 3; F: 4
