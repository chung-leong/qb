--TEST--
PBJ function declaration generation test
--FILE--
<?php

$path = "pbj/test.pbj";
$info = qb_extract($path, QB_PBJ_DECLARATION);
print_r($info);

?>
--EXPECT--
/**
 * test()	This is a description (Test Vendor)
 *
 * @engine	qb
 * @import	pbj/test.pbj
 *
 * @param	image	$dst
 * @param	image	$src
 * @param	int32	$value1	Description #1
 * @param	float32[x,y]	$value2	Description #2
 * @param	float32[3][3]	$value3	Description #3
 * @param	float32[r,g,b,a]	$value4	Description #4
 *
 * @return	void
 */
function test(&$dst, $src, $value1, $value2, $value3, $value4) {}
