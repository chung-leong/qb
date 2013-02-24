--TEST--
Class private method test
--FILE--
<?php 

class TestClass {

	/** @engine qb */
	private function private_function() {
		echo "Hello world";
	}
	
	public function test() {
		$this->private_function();
	}
}

$obj = new TestClass;
$obj->test();
@$obj->private_function();

?>
--EXPECT--
Hello world