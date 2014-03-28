--TEST--
Function call with namespace test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3') < 0) print 'skip feature not available';
?>
--FILE--
<?php 

namespace foo;

/**
 * @engine	qb 
 * @param	int32[4]	$a
 */
function test2($a) {
	echo "Test 2: $a\n";
}

/**
 * @engine	qb 
 * @param	int32[4]	$a
 */
function test1($a) {
	echo "Test 1: $a\n";
	test2($a);
}

test1(array(1, 2, 3, 4));

?>
--EXPECT--
Test 1: [1, 2, 3, 4]
Test 2: [1, 2, 3, 4]

