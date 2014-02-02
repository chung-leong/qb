--TEST--
Function argument by ref sub-array test (out of bound)
--FILE--
<?php

/**
 * @engine qb
 * @inline never
 * @param float64[][] $a
 * @return void
 */
function other_function(&$a) {
	$a += 6;
}

/**
 * @engine qb
 * @local float64[2][4][3] $a
 * @return void
 */
function test_function() {
	other_function($a[2]);
	echo "$a\n";
}

test_function();

?>
--EXPECTREGEX--
*.Fatal error.*line 19
