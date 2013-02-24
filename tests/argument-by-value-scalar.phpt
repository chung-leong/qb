--TEST--
Argument by value test (scalar)
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
function test_function($a, $b, $c, $d, $e, $f, $g, $h, $i, $j) {
	echo "$a $b $c $d $e $f $g $h $i $j";
}

qb_compile();

test_function(0xFF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x000FFFFFFFFFFFFF, 0x000FFFFFFFFFFFFF, 1.2e10, 1.1e20);

?>
--EXPECT--
-1 255 -1 65535 -1 4294967295 4503599627370495 4503599627370495 1.2E+10 1.1E+20