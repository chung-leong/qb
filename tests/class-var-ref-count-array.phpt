--TEST--
Class variable reference count test (array)
--FILE--
<?php 

class Test {

	/** @var int32[4] */
	public $a;

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

$b = array(1, 2, 3, 4);
$obj->a = $b;

function loop() {
	global $obj;
	for($i = 0; $i < 10; $i++) {
		$obj->run($i);
	}
}

loop();
print_r($obj);
print_r($b);

$b = array(3, 7, 2, 1);
$obj->a =& $b;

loop();
print_r($obj);
print_r($b);

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
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
)
[3, 7, 2, 1]
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
Array
(
    [0] => 9
    [1] => 9
    [2] => 9
    [3] => 9
)
