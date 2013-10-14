--TEST--
Class constant as array length test
--FILE--
<?php

class Bobo {

	const length = 5;	
}

class Test extends Bobo {

	/** @var int32[length] */
	protected $array;
	
	/**
	 * @engine	qb
	 * @return	void
	 */
	function show() {
		echo "$this->array\n";
	}
}

define('length', 7);

$obj = new Test();
$obj->show();

?>
--EXPECT--
[0, 0, 0, 0, 0]
