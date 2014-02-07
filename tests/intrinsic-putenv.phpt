--TEST--
Environment variable set test
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
	echo putenv("COW=CHICKEN"), "\n";
}

test_function();

if(DIRECTORY_SEPARATOR == '\\') {
	echo `echo %COW%`;
} else {
	echo `echo \$COW`;
}
echo getenv('COW'), "\n";

?>
--EXPECTREGEX--
1
CHICKEN
CHICKEN
