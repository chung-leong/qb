--TEST--
Random number test (assignment to array)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32[4]	$a
 * @local	int32[8]	$b
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = rand();
	echo "$a\n";
	
	$b = mt_rand();
	echo "$b\n";
}

qb_compile();

srand(0);
mt_srand(0);

test_function();

// results from rand() change depending on platform, but not mt_rand()

?>
--EXPECTREGEX--
\[\d+, \d+, \d+, \d+\]
\[963932192, 1273124119, 1535857466, 324735766, 1294424481, 1842424189, 1170127713, 1819459251\]

