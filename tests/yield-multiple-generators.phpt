--TEST--
Yield test (multiple generators)
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
 * @return	uint32
 * 
 */
function test_function($inc) {
	$k = 0;
	for($n = 1; $n <= 10; $n++) {
		$k += $inc;
		yield $n => $k;
	}
}

$gen1 = test_function(1);
$gen2 = test_function(2);
$gen3 = test_function(3);

while($gen1->valid() || $gen2->valid() || $gen3->valid()) {
	if($gen1->valid()) {
		echo "1: ", $gen1->key(), " => ", $gen1->current(), "\n";
		$gen1->next();
	}
	if($gen2->valid()) {
		echo "2: ", $gen2->key(), " => ", $gen2->current(), "\n";
		$gen2->next();
	}
	if($gen3->valid()) {
		echo "3: ", $gen3->key(), " => ", $gen3->current(), "\n";
		$gen3->next();
	}
}


?>
--EXPECT--
1: 1 => 1
2: 1 => 2
3: 1 => 3
1: 2 => 2
2: 2 => 4
3: 2 => 6
1: 3 => 3
2: 3 => 6
3: 3 => 9
1: 4 => 4
2: 4 => 8
3: 4 => 12
1: 5 => 5
2: 5 => 10
3: 5 => 15
1: 6 => 6
2: 6 => 12
3: 6 => 18
1: 7 => 7
2: 7 => 14
3: 7 => 21
1: 8 => 8
2: 8 => 16
3: 8 => 24
1: 9 => 9
2: 9 => 18
3: 9 => 27
1: 10 => 10
2: 10 => 20
3: 10 => 30
