--TEST--
Argument reference count test (array)
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @param	int32[4]	$a
 *
 * @return	void
 *
 */
function test_function(&$a) {
	echo "$a\n";
	$a = 0;
}

$a = array(1, 2, 3, 4);

function loop() {
	global $a;
	for($i = 0; $i < 10; $i++) {
		test_function($a);
	}
}

loop();
print_r($a);

?>
--EXPECT--
[1, 2, 3, 4]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
[0, 0, 0, 0]
Array
(
    [0] => 0
    [1] => 0
    [2] => 0
    [3] => 0
)

