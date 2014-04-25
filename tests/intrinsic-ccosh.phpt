--TEST--
Complex number hyperbolic cosine test
--FILE--
<?php 

/**
 * A test function
 *
 * @engine	qb 
 * @local	float32[2]			$a
 * @local	complex<float32>	$b
 * @local	float32[3][2]		$c
 *
 * @return	void
 *
 */
function test_function() {
	$a = $b = array(2, 4);
	$c[0] = array(3, -9);
	$c[1] = array(-13, 1);
	$c[2] = array(3.5, -2);
	
	$c = ccosh($c);
	echo ccosh($a), "\n";
	echo cosh($b), "\n";
	echo "$c\n";
	echo ccosh(array(0, 0)), "\n";
	echo ccosh(array(0, +INF)), "\n";
	echo ccosh(array(0, NAN)), "\n";
	echo ccosh(array(0.5, +INF)), "\n";
	echo ccosh(array(4.5, NAN)), "\n";
	echo ccosh(array(+INF, 0)), "\n";
	echo ccosh(array(+INF, 12)), "\n";
	echo ccosh(array(+INF, +INF)), "\n";
	echo ccosh(array(+INF, NAN)), "\n";
	echo ccosh(array(NAN, 0)), "\n";
	echo ccosh(array(NAN, 0.5)), "\n";
	echo ccosh(array(NAN, NAN)), "\n";
}

test_function();

?>
--EXPECTREGEX--
\[\-2\.459135, \-2\.744817\]
\[\-2\.459135, \-2\.744817\]
\[\[\-9\.172952, \-4\.128551\], \[119518\.5, \-186139\], \[\-6\.896729, \-15\.04217\]\]
\[1, 0\]
\[NAN, \-?0\]
\[NAN, \-?0\]
\[NAN, NAN\]
\[NAN, NAN\]
\[INF, 0\]
\[INF, \-INF\]
\[\-?INF, NAN\]
\[INF, NAN\]
\[NAN, 0\]
\[NAN, NAN\]
\[NAN, NAN\]

