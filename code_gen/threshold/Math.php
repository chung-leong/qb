<?php

class Abs_S08 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param int8[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class Abs_S16 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param int16[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class Abs_S32 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param int32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class Abs_S64 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param int64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class Abs_F32 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class ACos_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = acos($a);
		$e = time();
		return ($e - $s);
	}
}

class ACosh_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = acosh($a);
		$e = time();
		return ($e - $s);
	}
}

class ASin_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = asin($a);
		$e = time();
		return ($e - $s);
	}
}

class ASinh_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = asinh($a);
		$e = time();
		return ($e - $s);
	}
}

class ATan_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = atan($a);
		$e = time();
		return ($e - $s);
	}
}

class ATan2_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float32[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = 1 - $a;
		$s = time();
		$a = atan2($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ATanh_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = atanh($a);
		$e = time();
		return ($e - $s);
	}
}

class Ceil_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ceil($a);
		$e = time();
		return ($e - $s);
	}
}

class Clamp_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0, 1);
		$s = time();
		$a = clamp($a, 0, 0.5);
		$e = time();
		return ($e - $s);
	}
}

class Cos_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cos($a);
		$e = time();
		return ($e - $s);
	}
}

class Cosh_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cosh($a);
		$e = time();
		return ($e - $s);
	}
}

class DegreeToRadian_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = deg2rad($a);
		$e = time();
		return ($e - $s);
	}
}

class Exp_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = exp($a);
		$e = time();
		return ($e - $s);
	}
}

class Exp2_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = exp2($a);
		$e = time();
		return ($e - $s);
	}
}

class ExpM1_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = expm1($a);
		$e = time();
		return ($e - $s);
	}
}

class Floor_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = floor($a);
		$e = time();
		return ($e - $s);
	}
}

class Fract_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = fract($a);
		$e = time();
		return ($e - $s);
	}
}

class Hypot_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float32[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a * array(2, -3);
		$s = time();
		$a = hypot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class IsFinite_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_finite($a);
		$e = time();
		return ($e - $s);
	}
}

class IsInfinite_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_infinite($a);
		$e = time();
		return ($e - $s);
	}
}

class IsNaN_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_nan($a);
		$e = time();
		return ($e - $s);
	}
}

class Log_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log($a);
		$e = time();
		return ($e - $s);
	}
}

class Log1P_F32 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log1p($a);
		$e = time();
		return ($e - $s);
	}
}

class Log2_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log2($a);
		$e = time();
		return ($e - $s);
	}
}

class Log10_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log10($a);
		$e = time();
		return ($e - $s);
	}
}

class Mix_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float32[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a * array(2, -3);
		$s = time();
		$a = mix($a, $b, 0.5);
		$e = time();
		return ($e - $s);
	}
}

class Pow_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = pow($a, M_PI);
		$e = time();
		return ($e - $s);
	}
}

class RadianToDegree_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rad2deg($a);
		$e = time();
		return ($e - $s);
	}
}

class Rint_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a += array(0.3, 0.5, 0.7);
		$s = time();
		$a = rint($a);
		$e = time();
		return ($e - $s);
	}
}

class Round_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a += array(0.3, 0.5, 0.7);
		$s = time();
		$a = round($a);
		$e = time();
		return ($e - $s);
	}
}

class RSqrt_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rsqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class Sign_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sign($a);
		$e = time();
		return ($e - $s);
	}
}

class Sin_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sin($a);
		$e = time();
		return ($e - $s);
	}
}

class Sinh_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sinh($a);
		$e = time();
		return ($e - $s);
	}
}

class SmoothStep_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0.0001, 1);
		$s = time();
		$a = smooth_step(0, 1000, $a);
		$e = time();
		return ($e - $s);
	}
}

class Sqrt_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class Step_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0.0001, 1);
		$s = time();
		$a = step($a, 1000);
		$e = time();
		return ($e - $s);
	}
}

class Tan_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = tan($a);
		$e = time();
		return ($e - $s);
	}
}

class Tanh_F32 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float32[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = tanh($a);
		$e = time();
		return ($e - $s);
	}
}

class Abs_F64 {
	use SequentialNumbersWithNegative;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = abs($a);
		$e = time();
		return ($e - $s);
	}
}

class ACos_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = acos($a);
		$e = time();
		return ($e - $s);
	}
}

class ACosh_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = acosh($a);
		$e = time();
		return ($e - $s);
	}
}

class ASin_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = asin($a);
		$e = time();
		return ($e - $s);
	}
}

class ASinh_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = asinh($a);
		$e = time();
		return ($e - $s);
	}
}

class ATan_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = atan($a);
		$e = time();
		return ($e - $s);
	}
}

class ATan2_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = 1 - $a;
		$s = time();
		$a = atan2($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class ATanh_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = atanh($a);
		$e = time();
		return ($e - $s);
	}
}

class Ceil_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = ceil($a);
		$e = time();
		return ($e - $s);
	}
}

class Clamp_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0, 1);
		$s = time();
		$a = clamp($a, 0, 0.5);
		$e = time();
		return ($e - $s);
	}
}

class Cos_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cos($a);
		$e = time();
		return ($e - $s);
	}
}

class Cosh_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = cosh($a);
		$e = time();
		return ($e - $s);
	}
}

class DegreeToRadian_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = deg2rad($a);
		$e = time();
		return ($e - $s);
	}
}

class Exp_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = exp($a);
		$e = time();
		return ($e - $s);
	}
}

class Exp2_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = exp2($a);
		$e = time();
		return ($e - $s);
	}
}

class ExpM1_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = expm1($a);
		$e = time();
		return ($e - $s);
	}
}

class Floor_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = floor($a);
		$e = time();
		return ($e - $s);
	}
}

class Fract_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = fract($a);
		$e = time();
		return ($e - $s);
	}
}

class Hypot_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a * array(2, -3);
		$s = time();
		$a = hypot($a, $b);
		$e = time();
		return ($e - $s);
	}
}

class IsFinite_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_finite($a);
		$e = time();
		return ($e - $s);
	}
}

class IsInfinite_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_infinite($a);
		$e = time();
		return ($e - $s);
	}
}

class IsNaN_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local bool[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		array_resize($b, count($a));
		$s = time();
		$b = is_nan($a);
		$e = time();
		return ($e - $s);
	}
}

class Log_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log($a);
		$e = time();
		return ($e - $s);
	}
}

class Log1P_F64 {
	use SequentialInverses;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log1p($a);
		$e = time();
		return ($e - $s);
	}
}

class Log2_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log2($a);
		$e = time();
		return ($e - $s);
	}
}

class Log10_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = log10($a);
		$e = time();
		return ($e - $s);
	}
}

class Mix_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64[] $b
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$b = $a * array(2, -3);
		$s = time();
		$a = mix($a, $b, 0.5);
		$e = time();
		return ($e - $s);
	}
}

class Pow_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = pow($a, M_PI);
		$e = time();
		return ($e - $s);
	}
}

class RadianToDegree_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rad2deg($a);
		$e = time();
		return ($e - $s);
	}
}

class Rint_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a += array(0.3, 0.5, 0.7);
		$s = time();
		$a = rint($a);
		$e = time();
		return ($e - $s);
	}
}

class Round_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a += array(0.3, 0.5, 0.7);
		$s = time();
		$a = round($a);
		$e = time();
		return ($e - $s);
	}
}

class RSqrt_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rsqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class Sign_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sign($a);
		$e = time();
		return ($e - $s);
	}
}

class Sin_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sin($a);
		$e = time();
		return ($e - $s);
	}
}

class Sinh_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sinh($a);
		$e = time();
		return ($e - $s);
	}
}

class SmoothStep_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0.0001, 1);
		$s = time();
		$a = smooth_step(0, 1000, $a);
		$e = time();
		return ($e - $s);
	}
}

class Sqrt_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = sqrt($a);
		$e = time();
		return ($e - $s);
	}
}

class Step_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a *= array(-1, 0.0001, 1);
		$s = time();
		$a = step($a, 1000);
		$e = time();
		return ($e - $s);
	}
}

class Tan_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = tan($a);
		$e = time();
		return ($e - $s);
	}
}

class Tanh_F64 {
	use SequentialNumbers;

	/**
	 * @engine qb
	 * @param float64[] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = tanh($a);
		$e = time();
		return ($e - $s);
	}
}

?>
