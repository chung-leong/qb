--TEST--
Regex type assignment test
--FILE--
<?php

/**
 * A test function
 *
 * @engine	qb 
 *
 * @local	float64		$(width|height)
 * @local	int32		$[xy]
 * @return	void
 *
 */
function test_function() {
	$width = 3.5;
	$height = 4.5;
	$x = 13.2;
	$y = 17.8;
	echo "$x $y $width $height\n";
}

qb_compile();

test_function();

?>
--EXPECT--
13 17 3.5 4.5