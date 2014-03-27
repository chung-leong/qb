--TEST--
Argument by ref test (scalar)
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.2.1') < 0) print 'skip broken 64 bit output';
?>
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @param	int8		$a
 * @param	uint8		$b
 * @param	int16		$c
 * @param	uint16		$d
 * @param	int32		$e
 * @param	uint32		$f
 * @param	int64		$g
 * @param	uint64		$h
 * @param	float32		$i
 * @param	float64		$j
 *
 * @return	void
 *
 */
function test_function(&$a, &$b, &$c, &$d, &$e, &$f, &$g, &$h, &$i, &$j) {
	$a = 0xFF;
	$b = 0xFF;
	$c = 0xFFFF;
	$d = 0xFFFF;
	$e = -0x0FFFFFFF;
	$f = 0xFFFFFFFF;
	$g = -0x000000FFFFFFFFFF;
	$h = 0x000000FFFFFFFFFF;
	$i = 10000000000;
	$j = 1.0E+20;
}

test_function($a, $b, $c, $d, $e, $f, $g, $h, $i, $j);
echo "$a $b $c $d $e $f $g $h $i $j\n";

?>
--EXPECT--
-1 255 -1 65535 -268435455 4294967295 -1099511627775 1099511627775 10000000000 1.0E+20
