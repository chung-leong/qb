--TEST--
Large fixed length array initialization test
--FILE--
<?php

/**
 * @engine qb
 * @inline always
 * @param float64[5000] $a
 * @param float64		$v
 * @local uint32		$i
 * @return bool
 */
function check($a, $v) {
	for($i = 0; $i < count($a); $i++) {
		if($a[$i] != $v) {
			echo "Element $i not initialized to $v\n";
			return false;
		}
	}
	return true;
}

/**
 * @engine qb
 * @local float64[5000]	$a
 */
function test_function() {
	if(!check($a, 0)) {
		return;
	}
	$a = 5.0;
	if(!check($a, 5)) {
		return;
	}
	echo "Working as expected\n";
}

test_function();
test_function();

?>
--EXPECT--
Working as expected
Working as expected

