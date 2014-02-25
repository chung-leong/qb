--TEST--
Large fixed length array initialization test
--FILE--
<?php

/**
 * @engine qb
 * @local float64[5000]	$a
 * @local uint32		$i
 */
function test_function() {
	for($i = 0; $i < count($a); $i++) {
		if($a[$i] != 0) {
			echo "Element $i not initialized to zero\n";
			return;
		}
	}
	$a = 5.0;
	for($i = 0; $i < count($a); $i++) {
		if($a[$i] != 5.0) {
			echo "Element $i not set to to five\n";
			return;
		}
	}
	echo "Working as expected\n";
}

test_function();
test_function();

?>
--EXPECT--
Working as expected
Working as expected

