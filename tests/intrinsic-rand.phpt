--TEST--
Random number test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.2') < 0) print 'skip old mt_rand imnplementation';
?>
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int8	$a
 * @local	int16	$b
 * @local	int32	$c
 * @local	int64	$d
 * @local	uint8	$e
 * @local	uint16	$f
 * @local	uint32	$g
 * @local	uint64	$h
 * 
 * @return	void
 * 
 */
function test_function() {
	$a = rand();
	$b = rand();
	$c = rand();
	$d = rand();
	$e = rand();
	$f = rand();
	$g = rand();
	$h = rand();
	
	echo "$a $b $c $d $e $f $g $h\n";
	
	$a = mt_rand();
	$b = mt_rand();
	$c = mt_rand();
	$d = mt_rand();
	$e = mt_rand();
	$f = mt_rand();
	$g = mt_rand();
	$h = mt_rand();
	
	echo "$a $b $c $d $e $f $g $h\n";
}

srand(0);
mt_srand(0);

test_function();

// results from rand() change depending on platform, but not mt_rand()

?>
--EXPECTREGEX--
\d+ \d+ \d+ \d+ \d+ \d+ \d+ \d+
57 19426 1535857466 1394729497400357698 219 35709 3638918502 7815051610339532498
