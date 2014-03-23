--TEST--
Array element reference test
--FILE--
<?php

/** 
  * @engine qb 
  * @param int32[4]			$a
  * @param float64[2][2]	$b
  */
function test(&$a, &$b) {
	$a = array(1, 2, 3, 4);
	$b = array(3, 5, 7, 9);
}

$a[0] =& $a0;
$a[1] =& $a1;
$a[2] =& $a2;
$a[3] =& $a3;

$b[0][0] =& $b00;
$b[0][1] =& $b01;
$b[1][0] =& $b10;
$b[1][1] =& $b11;

test($a, $b);

echo "$a0 $a1 $a2 $a3\n";
echo "$b00 $b01 $b10 $b11\n";

?>
--EXPECT--
1 2 3 4
3 5 7 9

