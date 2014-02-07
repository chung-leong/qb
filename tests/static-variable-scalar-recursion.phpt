--TEST--
Static variable test (scalar, recursion)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @static	int32	$.*
 * 
 * @return	void
 * 
 */
function test_function() {
	static $a = 2000;
	echo "$a\n";
	$a++;
	if($a < 2010) {
		test_function();
	} else {
		echo "Final: $a\n";
	}
}

test_function();

?>
--EXPECT--
2000
2001
2002
2003
2004
2005
2006
2007
2008
2009
Final: 2010
