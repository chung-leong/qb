--TEST--
Class variable reference count test (array)
--FILE--
<?php 

class Test {

	/** @var int32[4] */
	public $a = array(1, 2, 3, 4);

	/** 
	 * @engine qb 
	 * @param int32	$i
	 * */
	public function run($i) {
		echo "$this->a\n";
		$this->a = $i;
	}
}

$obj = new Test;

function loop() {
	global $obj;
	for($i = 0; $i < 10; $i++) {
		$obj->run($i);
	}
}

loop();
print_r($obj);

?>
--EXPECT--
[1, 2, 3, 4]
[0, 0, 0, 0]
[1, 1, 1, 1]
[2, 2, 2, 2]
[3, 3, 3, 3]
[4, 4, 4, 4]
[5, 5, 5, 5]
[6, 6, 6, 6]
[7, 7, 7, 7]
[8, 8, 8, 8]
Test Object
(
    [a] => Array
        (
            [0] => 9
            [1] => 9
            [2] => 9
            [3] => 9
        )

)

