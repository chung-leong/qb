--TEST--
Coroutine test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	uint32	$.*
 * @param	uint32	$.*
 * @receive	uint32
 * @return	uint32
 * 
 */
function test_function() {
	$k = 0;
	$x = 1;
	while($x) {
		$k += 10;
		$x = (yield $k);
		echo "Received $x\n";
	}
}

$gen = test_function();
while($gen->valid()) {
	$v = $gen->current();
	echo "Current: $v\n";
	$gen->send(($v < 100) ? $v * 10 : 0);
}

?>
--EXPECT--
Current: 10
Received 100
Current: 20
Received 200
Current: 30
Received 300
Current: 40
Received 400
Current: 50
Received 500
Current: 60
Received 600
Current: 70
Received 700
Current: 80
Received 800
Current: 90
Received 900
Current: 100
Received 0
