--TEST--
Bug #20
--SKIPIF--
<?php 
	if(!function_exists('gc_enable')) print 'skip gc function not available';
?>
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @param	int32[4][4]	$a
 *
 * @return	void
 *
 */
function test(&$a) {
	$a[0] = 2;
	$a[1] = 4;
	$a[2] = 6;
	$a[3] = 8;
}

gc_enable();
for($i = 0; $i < 1000; $i++) {
	if($i % 16 == 0) {
		unset($a);
		$a = array(
			array(1, 2, 3, 4), 
			array(1, 2, 3, 4), 
			array(1, 2, 3, 4), 
			array(1, 2, 3, 4), 
		);
	}

	test($a);
	gc_collect_cycles();

	// do some random stuff
	if($i % 37 == 0) {
		$other_stuff = array();
	} else if($i % 3 == 0) {
		$other_stuff[] = rand();
	} else if($i % 2 == 0) {
		$other_stuff[] = strtoupper("hello");
	} else {
		$other_stuff[] = true;
	}
}
print_r($a);

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => 2
            [1] => 2
            [2] => 2
            [3] => 2
        )

    [1] => Array
        (
            [0] => 4
            [1] => 4
            [2] => 4
            [3] => 4
        )

    [2] => Array
        (
            [0] => 6
            [1] => 6
            [2] => 6
            [3] => 6
        )

    [3] => Array
        (
            [0] => 8
            [1] => 8
            [2] => 8
            [3] => 8
        )

)
