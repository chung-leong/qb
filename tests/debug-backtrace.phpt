--TEST--
Debug backtrace test
--FILE--
<?php

/**
 * @engine qb
 * @param uint32	$a
 * @param uint32	$b
 */
function test_function1($a, $b) {
	echo "Test function1 ($a, $b)\n";
	test_function2(3, 4);
}

/**
 * @engine qb
 * @param float32	$a
 * @param float32	$b
 */
function test_function2($a, $b) {
	echo "Test function2 ($a, $b)\n";
	test_function3(array(1, 2, 3, 4));
}

/**
 * @engine qb
 * @param float32[4]	$a
 */
function test_function3($a) {
	echo "Test function3 ($a)\n";
	debug_print_backtrace();
}

ini_set("qb.allow_debug_backtrace", true);

test_function1(1, 2);

?>
--EXPECTREGEX--
Test function1 \(1, 2\)
Test function2 \(3, 4\)
Test function3 \(\[1, 2, 3, 4\]\)
#0  test_function3\(Array \(\[0\] => 1,\[1\] => 2,\[2\] => 3,\[3\] => 4\)\) called at \[.*:20\]
#1  test_function2\(3, 4\) called at \[.*:10\]
#2  test_function1\((uint32 Object \(\[value\] => 1\), uint32 Object \(\[value\] => 2\)|1, 2)\) called at \[.*:36\]
