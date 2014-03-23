--TEST--
Multiply by literal array test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.4') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/** 
  * @engine qb
  * @local float64[4] $.*
  */
function a() {
	$b = [1, 2, 3, 4];
	$a = $b * [1, 2, 3, 4];
	echo "$a\n";
}

a();

?>
--EXPECT--
[1, 4, 9, 16]
