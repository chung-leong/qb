--TEST--
Exception test
--FILE--
<?php

function other_function($s) {
	throw new Exception($s);
}

/** @engine	qb */
function test_function() {
	other_function("hello");
	echo "Not reachable";
}

try {
	test_function();
} catch(Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
hello
