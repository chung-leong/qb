--TEST--
Function argument by value sub-array test (out of bound)
--FILE--
<?php

/**
 * @engine qb
 * @inline always
 * @param float64[4][3] $a
 * @return void
 */
function other_function(&$a) {
	echo "$a\n";
}

/**
 * @engine qb
 * @local float64[2][4][3] $a
 * @return void
 */
function test_function() {
	$a[1] += 6;
	other_function($a[2]);
}

test_function();

?>
--EXPECTREGEX--
.*Fatal error.*line 20

