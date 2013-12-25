--TEST--
Environment variable retrieval test
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * 
 * @return	void
 * 
 */
function test_function() {
	echo getenv("PATH"), "\n";
}

test_function();

?>
--EXPECTREGEX--
\S+
