--TEST--
Method call with namespace test
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3') < 0) print 'skip feature not available';
?>
--FILE--
<?php 

namespace foo;

class TestClass1 {

	/**
	 * @engine qb
	 * @param int32		$a
	 * @param int16[]	$b
	 * @param string	$c
	 */
	public function other_function($a, $b, $c) {
		echo "$a, $b, $c\n";
	}
}

class TestClass2 {

	/**
	 * @engine qb
	 * @local int32		$a
	 * @local int16[3]	$b
	 * @local string	$c
	 */
	public function test() {
		$a = 1234;
		$b = array(99, 66, 77);
		$c = "something";
		TestClass1::other_function($a, $b, $c);
	}
}

$obj = new TestClass2;

$obj->test();

?>
--EXPECT--
1234, [99, 66, 77], something
