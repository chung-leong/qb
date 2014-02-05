<?php

class ApplyPremultiplication_F32_X4 {
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

class HSL2RGB_F32_X4 {
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

class HSV2RGB_F32_X4 {
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

class RemovePremultiplication_F32_X4 {
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

class RGB2HSL_F32_X4 {
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

class RGB2HSV_F32_X4 {
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

class SampleBilinear_F32_X1 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][1] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
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

class SampleBilinear_F32_X2 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][2] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
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

class SampleBilinear_F32_X3 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][3] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
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

class SampleBilinear_F32_X4 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
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

class SampleNearest_F32_X1 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][1] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F32_X2 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][2] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F32_X3 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][3] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F32_X4 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float32[][][4] $a
	 * @param float32[]	$(x|y)
	 * @local float32[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class ApplyPremultiplication_F64_X4 {
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

class HSL2RGB_F64_X4 {
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

class HSV2RGB_F64_X4 {
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

class RemovePremultiplication_F64_X4 {
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

class RGB2HSL_F64_X4 {
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

class RGB2HSV_F64_X4 {
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

class SampleBilinear_F64_X1 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][1] $a
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

class SampleBilinear_F64_X2 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][2] $a
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

class SampleBilinear_F64_X3 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][3] $a
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

class SampleBilinear_F64_X4 {
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

class SampleNearest_F64_X1 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][1] $a
	 * @param float64[]	$(x|y)
	 * @local float64[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F64_X2 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][2] $a
	 * @param float64[]	$(x|y)
	 * @local float64[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F64_X3 {
	use Sampling;

	/**
	 * @engine qb
	 * @param float64[][][3] $a
	 * @param float64[]	$(x|y)
	 * @local float64[][4]	$r
	 * @local float64 $(s|e)
	 * @return float64
	 */
	function test($a, $x, $y) {
		$s = time();
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

class SampleNearest_F64_X4 {
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
		$r = sample_nearest($a, $x, $y);
		$e = time();
		return ($e - $s);
	}
}

?>
