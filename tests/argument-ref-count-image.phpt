--TEST--
Argument reference count test (image)
--EXTENSIONS--
gd
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @param	image	$a
 *
 * @return	void
 *
 */
function test_function(&$a) {
	echo "$a\n";
	$a = 1;
}

$a = imagecreatetruecolor(2, 2);

function loop() {
	global $a;
	for($i = 0; $i < 10; $i++) {
		test_function($a);
	}
}

loop();
echo "$a\n";

?>
--EXPECT--
[[[0, 0, 0, 1], [0, 0, 0, 1]], [[0, 0, 0, 1], [0, 0, 0, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
[[[1, 1, 1, 1], [1, 1, 1, 1]], [[1, 1, 1, 1], [1, 1, 1, 1]]]
Resource id #4

