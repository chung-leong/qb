--TEST--
Array element increment test (scalar)
--FILE--
<?php 

/** 
  * @engine qb 
  * @local int32[4]		$a
  */
function test() {
	$a = array(1, 2, 3, 4);
	$a[3] += 4;
	$a[1] += 4;
	echo "$a\n";
}

test();

?>
--EXPECT--
[1, 6, 3, 8]
