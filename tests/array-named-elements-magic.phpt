--TEST--
Array with named elements test,  object with getter and setter functions
--FILE--
<?php

/**
 * A test function
 * 
 * @engine	qb
 * @param	float32[ r,g,b,a ]	$a
 * 
 * @return	void
 * 
 */
function test_function(&$a) {
	$a->r /= 2;
	$a->g /= 2;
	$a->b /= 2;
	$a->a = 1;
}

qb_compile();

class RGBA {

	private $components = array();
	
	public function __get($name) {
		switch($name) {
			case 'r':
				return @$this->components['RED'];
				break;
			case 'g':
				return @$this->components['GREEN'];
				break;
			case 'b':
				return @$this->components['BLUE'];
				break;
			case 'a':
				return @$this->components['ALPHA'];
				break;
		}
	}

	public function __set($name, $value) {
		switch($name) {
			case 'r':
				$this->components['RED'] = $value;
				break;
			case 'g':
				$this->components['GREEN'] = $value;
				break;
			case 'b':
				$this->components['BLUE'] = $value;
				break;
			case 'a':
				$this->components['ALPHA'] = $value;
				break;
		}
	}
	
	public function __toString() {
		return print_r($this->components, true);
	}
}

$object = new RGBA;
$object->r = 1;
$object->g = 0.25;
$object->b = 0.5;

test_function($object);

echo $object;

?>
--EXPECT--
Array
(
    [RED] => 0.5
    [GREEN] => 0.125
    [BLUE] => 0.25
    [ALPHA] => 1
)
