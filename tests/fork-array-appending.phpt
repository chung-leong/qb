--TEST--
Array appending test (in forked path)
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @local	int32[*][4]	$a
 * @local	int32[4]	$v
 * @local	uint32		$(i|j)
 * @return	void
 * 
 */
function test_function() {
	$i = fork(8);
	for($j = 0; $j < 100; $j++) {
		$v = $i;
		$a[] = $v;
	}
	echo "$a\n";
}

test_function();


?>
--EXPECTREGEX--
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
\[(\[\d, \d, \d, \d\], ){99}\[\d, \d, \d, \d\]\]
