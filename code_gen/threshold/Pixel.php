<?php

class RGB2HSL_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb2hsl($a);
		$e = time();
		return ($e - $s);
	}
}

class RGB2HSV_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb2hsv($a);
		$e = time();
		return ($e - $s);
	}
}

class HSL2RGB_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb2hsl($a);
		$s = time();
		$a = hsl2rgb($a);
		$e = time();
		return ($e - $s);
	}
}

class HSV2RGB_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb2hsv($a);
		$s = time();
		$a = hsv2rgb($a);
		$e = time();
		return ($e - $s);
	}
}

class ApplyPremultiplication_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb_premult($a);
		$e = time();
		return ($e - $s);
	}
}

class RemovePremultiplication_F32 {
	use Image;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb_premult($a);
		$s = time();
		$a = rgb_demult($a);
		$e = time();
		return ($e - $s);
	}
}

class RGB2HSL_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb2hsl($a);
		$e = time();
		return ($e - $s);
	}
}

class RGB2HSV_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb2hsv($a);
		$e = time();
		return ($e - $s);
	}
}

class HSL2RGB_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb2hsl($a);
		$s = time();
		$a = hsl2rgb($a);
		$e = time();
		return ($e - $s);
	}
}

class HSV2RGB_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb2hsv($a);
		$s = time();
		$a = hsv2rgb($a);
		$e = time();
		return ($e - $s);
	}
}

class ApplyPremultiplication_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$s = time();
		$a = rgb_premult($a);
		$e = time();
		return ($e - $s);
	}
}

class RemovePremultiplication_F64 {
	use Image;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a) {
		$a = rgb_premult($a);
		$s = time();
		$a = rgb_demult($a);
		$e = time();
		return ($e - $s);
	}
}

class SampleBilinear_F64 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][4] $a
	 * @param float64[]	$(x|y)
	 * @local float64[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_bilinear($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

?>
