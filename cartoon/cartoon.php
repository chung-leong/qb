<?php

class OilPaintFilter {

	/** @var int32 */
	public $levels = 25;
	
	/** @var int32 */
	public $filterSize = 5;

	/**
	 * @engine qb
	 *
	 * @param image					$dst
	 * @param image					$src
	 *
	 * @local float32[*][r,g,b,a]	$bin
	 * @local float32				$max_intensity
	 * @local int32					$(current_index|max_index)
	 * @local int32					$(filter_x|filter_y|filter_offset)
	 * @local int32					$(x|y|x_limit|y_limit)
	 * @local int32					$(width|height)
	 * @local float32[r,g,b,a]		$pixel
	 * @local float32				$multiplier
	 */
	public function filter(&$dst, $src) {
		$height = count($src);
		$width = count($src[0]);
	
		$filter_offset = ($this->filterSize - 1) / 2;
		$multiplier = ($this->levels - 1) / 3;
		
		$y_limit = $height - $filter_offset;
		$x_limit = $width - $filter_offset;
		
		array_resize($dst, $height, $width);
		
		$y = fork($y_limit - $filter_offset) + $filter_offset;
		
		for($x = $filter_offset; $x < $x_limit; $x++) {
			$current_index = $max_index = 0;
			$max_intensity = 0;
			
			unset($bin);
			
			for($filter_y = -$filter_offset; $filter_y <= $filter_offset; $filter_y++) {
				for($filter_x = -$filter_offset; $filter_x <= $filter_offset; $filter_x++) {
				
					$pixel = $src[$y + $filter_y][$x + $filter_x];
				
					$current_index = round(($pixel->r + $pixel->g + $pixel->b) * $multiplier);
					$bin[$current_index] += $pixel;
					
					if($bin[$current_index]->a > $max_intensity) {
						$max_intensity = $bin[$current_index]->a;
						$max_index = $current_index;
					}
				}
			}

			$dst[$y][$x] = $bin[$max_index] / $max_intensity;
		}
	}
}

class GradientBasedEdgeDetectionFilter {

	/** @var float32 */
	public $threshold = 0.25;
	
	/**
	 * @engine qb
	 *
	 * @param image4				$dst
	 * @param image3				$src
	 *
	 * @local int32					$(x|y|x_limit|y_limit)
	 * @local int32					$(width|height)
	 * @local float32[3]			$(pixel1|pixel2)
	 * @local float32				$(gradient_value1|gradient_value2|gradient_value3)
	 * @local bool					$exceeds_threshold
	 */
	 public function filter(&$dst, $src) {
		$height = count($src);
		$width = count($src[0]);
	 
		$y_limit = $height - 1;
		$x_limit = $width - 1;
		
		array_resize($dst, $height, $width);
		
		$y = fork($y_limit - 1) + 1;
		
		for($x = 1; $x < $x_limit; $x++) {
			// Horizontal Gradient
			$pixel1 = $src[$y][$x - 1];
			$pixel2 = $src[$y][$x + 1];
			$gradient_value1 = array_sum(abs($pixel1 - $pixel2));
			$exceeds_threshold = $gradient_value1 >= $this->threshold;
			
			if(!$exceeds_threshold) {
				// Vertical Gradient
				$pixel1 = $src[$y - 1][$x];
				$pixel2 = $src[$y + 1][$x];
				$gradient_value2 = array_sum(abs($pixel1 - $pixel2));
				$exceeds_threshold = $gradient_value2 >= $this->threshold;
				
				if(!$exceeds_threshold) {
					// Combined
					$gradient_value3 = ($gradient_value1 + $gradient_value2) * 0.5;
					$exceeds_threshold = $gradient_value3 >= $this->threshold;
					
					if(!$exceeds_threshold) {
						// Diagonal Gradient : NW-SE
						$pixel1 = $src[$y - 1][$x - 1];
						$pixel2 = $src[$y + 1][$x + 1];
						$gradient_value1 = array_sum(abs($pixel1 - $pixel2));
						$exceeds_threshold = $gradient_value1 >= $this->threshold;
						
						if(!$exceeds_threshold) {
							// Diagonal Gradient : NE-SW
							$pixel1 = $src[$y + 1][$x - 1];
							$pixel2 = $src[$y - 1][$x + 1];
							$gradient_value2 = array_sum(abs($pixel1 - $pixel2));
							$exceeds_threshold = $gradient_value2 >= $this->threshold;
							
							if(!$exceeds_threshold) {
								// Combined
								$gradient_value3 = ($gradient_value1 + $gradient_value2) * 0.5;
								$exceeds_threshold = $gradient_value3 >= $this->threshold;
							}
						}
					}
				}
			}
			
			if($exceeds_threshold) {
				$dst[$y][$x] = array(0, 0, 0, 1.0);
			}
		}
	}

}

$folder = dirname(__FILE__);
$input = imagecreatefrompng("$folder/ipad.png");

$filter1 = new OilPaintFilter;
$filter1->filter($output, $input);

$filter2 = new GradientBasedEdgeDetectionFilter;
$filter2->filter($output, $output);

header("Content-Type: image/png");
imagesavealpha($output, true);
imagepng($output);

?>