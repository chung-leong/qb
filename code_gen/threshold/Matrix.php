<?php

class CrossProduct_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = cross($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class CrossProduct_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float32[][3] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = cross($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class CrossProduct_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float32[][4] $b
	 * @local float32[][4] $c
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$c = $a - 3;
		$s = time();
		$b = cross($a, $b, $c);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][2][2] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float32[][3][3] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float32[][4][4] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Distance_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2));
		$e = time();
		return ($e - $s);
	}
}

class Distance_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2, 3));
		$e = time();
		return ($e - $s);
	}
}

class Distance_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2, 3, 4));
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[][2] $b
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float32[][3] $b
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float32[][4] $b
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float32[][3] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float32[][4] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][2][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float32[][3][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float32[][4][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float32[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][2][2] $a
	 * @local float32[][2][2] $b
	 * @local float32[][2][2] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float32[][3][3] $a
	 * @local float32[][3][3] $b
	 * @local float32[][3][3] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float32[][4][4] $a
	 * @local float32[][4][4] $b
	 * @local float32[][4][4] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64[2][2] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2), array(3, 4));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64[3][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64[4][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12), array(13, 14, 15, 16));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64[2][2] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2), array(3, 4));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64[3][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64[4][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12), array(13, 14, 15, 16));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0));
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0, -1));
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0, -1, 8));
		$e = time();
		return ($e - $s);
	}
}

class Refract_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class Refract_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0, -1), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class Refract_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0, -1, 8), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float32[][2] $a
	 * @local float64[2][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float32[][3] $a
	 * @local float64[3][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][4] $a
	 * @local float64[4][5] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4, 5), array(6, 7, 8, 9, 10), array(11, 12, 13, 14, 15), array(16, 17, 18, 19, 20));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F32_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float32[][2][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F32_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float32[][3][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F32_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float32[][4][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

class CrossProduct_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = cross($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class CrossProduct_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[][3] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = cross($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class CrossProduct_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[][4] $b
	 * @local float64[][4] $c
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$c = $a - 3;
		$s = time();
		$b = cross($a, $b, $c);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][2][2] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float64[][3][3] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Determinant_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float64[][4][4] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = det($a);
		$e = time();
		return ($e - $s);
	}
}

class Distance_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2));
		$e = time();
		return ($e - $s);
	}
}

class Distance_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2, 3));
		$e = time();
		return ($e - $s);
	}
}

class Distance_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = distance($a, array(1, 2, 3, 4));
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[][2] $b
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[][3] $b
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class DotProduct_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[][4] $b
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		array_resize($r, count($a));
		$s = time();
		$r = dot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[][2] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[][3] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class FaceForward_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[][4] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 3;
		$s = time();
		$b = faceforward($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][2][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float64[][3][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class InvertMatrix_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float64[][4][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = inverse($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class Length_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($r, count($a));
		$s = time();
		$r = length($a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][2][2] $a
	 * @local float64[][2][2] $b
	 * @local float64[][2][2] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float64[][3][3] $a
	 * @local float64[][3][3] $b
	 * @local float64[][3][3] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByMatrix_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float64[][4][4] $a
	 * @local float64[][4][4] $b
	 * @local float64[][4][4] $r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a + 2;
		array_resize($r, count($a));
		$s = time();
		$r = mm_mult($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[2][2] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2), array(3, 4));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[3][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyMatrixByVector_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[4][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12), array(13, 14, 15, 16));
		$s = time();
		$a = mv_mult($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[2][2] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2), array(3, 4));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[3][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6), array(7, 8, 9));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class MultiplyVectorByMatrix_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[4][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12), array(13, 14, 15, 16));
		$s = time();
		$a = vm_mult($a, $m);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Normalize_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = normalize($a);
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0));
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0, -1));
		$e = time();
		return ($e - $s);
	}
}

class Reflect_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = reflect($a, array(1, 0, -1, 8));
		$e = time();
		return ($e - $s);
	}
}

class Refract_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class Refract_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0, -1), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class Refract_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = refract($a, array(1, 0, -1, 8), 0.7);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 2;

	/**
	 * @engine qb
	 * @param float64[][2] $a
	 * @local float64[2][3] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3), array(4, 5, 6));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 3;

	/**
	 * @engine qb
	 * @param float64[][3] $a
	 * @local float64[3][4] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4), array(5, 6, 7, 8), array(9, 10, 11, 12));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransformVector_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][4] $a
	 * @local float64[4][5] $m
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$m = array(array(1, 2, 3, 4, 5), array(6, 7, 8, 9, 10), array(11, 12, 13, 14, 15), array(16, 17, 18, 19, 20));
		$s = time();
		$a = transform($m, $a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F64_X2 {
	use SequentialNumbers;

	var $vector_size = 4;

	/**
	 * @engine qb
	 * @param float64[][2][2] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F64_X3 {
	use SequentialNumbers;

	var $vector_size = 9;

	/**
	 * @engine qb
	 * @param float64[][3][3] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

class TransposeMatrix_F64_X4 {
	use SequentialNumbers;

	var $vector_size = 16;

	/**
	 * @engine qb
	 * @param float64[][4][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = transpose($a);
		$e = time();
		return ($e - $s);
	}
}

?>
