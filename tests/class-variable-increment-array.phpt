--TEST--
Class variable increment test (array)
--FILE--
<?php 

class TestClass {
	
	/** @var float32[4] */
	public $b = array(1, 2, 3, 4);

	/** @engine qb */
	public function test() {
		echo "Inside qb: $this->b\n";
		$this->b++;
		--$this->b;
		++$this->b;
	}
}

$obj = new TestClass;

$obj->test();

echo "Outside qb:\n";
print_r($obj->b);

?>
--EXPECT--
Inside qb: [1, 2, 3, 4]
Outside qb:
Array
(
    [0] => 2
    [1] => 3
    [2] => 4
    [3] => 5
)
