--TEST--
Array compare test (equal)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[4]	$[abc]
 * @local	int32[8]	$[eg]
 * @local	int32[10]	$[f]
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = array(1, 2, 3, 4);
	$b = array(1, 2, 3, 4);
	$c = array(2, 3, 4, 5);
	$e = array(1, 2, 3, 4, 1, 2, 3, 4);
	$f = array(1, 2, 3, 4, 1, 2, 3, 4, 1, 2);
	$g = array(1, 2, 3, 4, 2, 3, 4, 5);
	
	if($a != $b) {
		echo "$a != $b\n";
	} else {
		echo "$a == $b\n";
	}
	if($a != $c) {
		echo "$a != $c\n";
	} else {
		echo "$a == $c\n";
	}
	
	if($a != $e) {
		echo "$a != $e\n";
	} else {
		echo "$a == $e\n";
	}
	if($e != $a) {
		echo "$e != $a\n";
	} else {
		echo "$e == $a\n";
	}
	
	if($a != $f) {
		echo "$a != $f\n";
	} else {
		echo "$a == $f\n";
	}
	if($f != $a) {
		echo "$f != $a\n";
	} else {
		echo "$f == $a\n";
	}
	
	if($a != $g) {
		echo "$a != $g\n";
	} else {
		echo "$a == $g\n";
	}
	if($g != $a) {
		echo "$g != $a\n";
	} else {
		echo "$g == $a\n";
	}
	
	if($e != $f) {
		echo "$e != $f\n";
	} else {
		echo "$e == $f\n";
	}
	if($f != $e) {
		echo "$f != $e\n";
	} else {
		echo "$f == $e\n";
	}
}

test_function();

?>
--EXPECT--
[1, 2, 3, 4] == [1, 2, 3, 4]
[1, 2, 3, 4] != [2, 3, 4, 5]
[1, 2, 3, 4] == [1, 2, 3, 4, 1, 2, 3, 4]
[1, 2, 3, 4, 1, 2, 3, 4] == [1, 2, 3, 4]
[1, 2, 3, 4] == [1, 2, 3, 4, 1, 2, 3, 4, 1, 2]
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2] == [1, 2, 3, 4]
[1, 2, 3, 4] != [1, 2, 3, 4, 2, 3, 4, 5]
[1, 2, 3, 4, 2, 3, 4, 5] != [1, 2, 3, 4]
[1, 2, 3, 4, 1, 2, 3, 4] == [1, 2, 3, 4, 1, 2, 3, 4, 1, 2]
[1, 2, 3, 4, 1, 2, 3, 4, 1, 2] == [1, 2, 3, 4, 1, 2, 3, 4]
