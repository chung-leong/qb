--TEST--
Yield test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.5.0') < 0) print 'skip feature not available';
?>
--FILE--
<?php

/**
 * @engine qb
 * @inline never
 * @param image			$(dst|src)
 * @param float32		$intensity
 *
 * @local float32[4][4]	$YIQMatrix
 * @local float32[4][4]	$inverseYIQ
 * @local float32[4]	$k
 */
function sepia(&$dst, $src, $intensity) {
	ini_set("qb.column_major_matrix", true);
	
	$YIQMatrix = array(
		array(0.299,  0.596,  0.212, 0.000),
		array(0.587, -0.275, -0.523, 0.000),
		array(0.114, -0.321,  0.311, 0.000),
		array(0.000,  0.000,  0.000, 1.000),
	);
	
	$src = mv_mult($YIQMatrix, $src);
	$src *= array(1, 0, 0, 1);				// clear I and Q
	$src += array(0, $intensity, 0, 0);		// set I to intensity
	$inverseYIQ = array(
		array(1.0,    1.0,    1.0,    0.0),
		array(0.956, -0.272, -1.10,   0.0),
		array(0.621, -0.647,  1.70,   0.0),
		array(0.0,    0.0,    0.0,    1.0),
	);	
	$dst = mv_mult($inverseYIQ, $src);
	
	ini_restore("qb.column_major_matrix");
}

/**
 * @engine	qb
 * @param	image	$src
 * @local	image	$dst
 * @local	float32	$intensity
 * @return	image
 * 
 */
function yield_sepia($src) {
	$intensity = 0;
	while($intensity < 0.5) {
		$intensity += 0.1;
		sepia($dst, $src, $intensity);
		yield $dst;
	}
}

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/malgorzata_socha.png");
$correct_path = "$folder/output/sepia-2.correct.png";

foreach(yield_sepia($image) as $index => $image) {
	echo "$index => $image\n";
}

?>
--EXPECT--
