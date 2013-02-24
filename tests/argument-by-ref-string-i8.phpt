--TEST--
Argument by reference test (byte string, I08)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	int8[]	$a
 * @local	int8	$n
 * @local	uint32	$i
 * 
 * @return	void
 * 
 */
function test_function(&$a) {
	foreach($a as $i => $n) {
		$a[$i] = $n + 1;
	}
}

qb_compile();

$data = pack('c*', 123, 124, 77, 54, 72);

test_function($data);

$array = unpack('c*', $data);

print_r($array);

?>
--EXPECT--
Array
(
    [1] => 124
    [2] => 125
    [3] => 78
    [4] => 55
    [5] => 73
)