--TEST--
Anonymous function test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3.0') < 0) print 'skip closure not available';
?>
--FILE--
<?php

$a = function($a, $b) {
	$a += 5;
	$b *= 5;
	echo "$a $b\n";
	return $a + $b;
};

$decl = [
	'engine' => 'qb',
	'param' => [
		'int32 $a', 
		'int32 $b',
	],
	'return' => 'uint32'
];

qb_compile($a, $decl);

echo $a(7, 6), "\n";

?>
--EXPECT--
12 30
42
