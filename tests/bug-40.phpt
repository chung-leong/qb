--TEST--
Bug #40
--SKIPIF--
<?php 
	if(version_compare(PHP_VERSION, '5.3') < 0) print 'skip feature not available';
?>
--FILE--
<?php

namespace ns;

class Test {

	/**
	 * @engine qb
	 * @param float32[][3]		$(image1|image2)
	 * @local int32				$i
	 * @return boolean
	 */
	public function compareImages($image1, $image2) {
		if(count($image1) != count($image2)) {
			return false;
		}
		for($i = 0; $i < count($image1); $i++) {
			if($image1[$i] != $image2[$i]) {
				return false;
			}
		}
		return true;
	}
}

$obj = new Test;
echo $obj->compareImages(array(), array());

?>
--EXPECT--
1
