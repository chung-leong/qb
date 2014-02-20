--TEST--
Use case: Droste Effect (multicore)
--SKIPIF--
<?php 
	if(!function_exists('imagepng')) print 'skip PNG function not available';
?>
--FILE--
<?php

// Landscape Generator
// -----------------------
// By Tom Beddard
// http://www.subblue.com
//
//
// Notes:
// -----------------------
//
// A ray vector for each pixel is fired into the scene. The ray steps a distance
// epsilon and then checks it's height above the terrain. This process continues
// until the ray height drops below that of the terrain. At this intersection
// point we calculate the normal vector then the dot product of this with a 
// normalised vector to the sun position to calculate the diffuse shading
// factor.
//
// This version uses a composition of different frequency sine functions to
// create a noise-like height map to for the terrain displacement.
// I also created versions that use Perlin and Simplex noise functions to generate 
// more realistic height maps, but that is too slow and often locks up the GPU 
// requiring a hard reset on my NVidia 8800GS iMac!
// 
// Standard landscape ray-marching techniques store the ray depth of the pixel
// below the one being calculated (as you it will correspond to a point on
// the terrain closer to the camera than the pixel above it), which means subsequent
// ray tracing can start from where the previous ray stopped saving a huge number of
// iterations.
// Unfortunately because the way the GPU renders many pixels in parallel you can't
// query the result of the pixel below the one you are calculating so it isn't
// possible to use this optimisation. The ray marching has to be done with brute
// force! 
// 
//
// References:
// ------------------------
//
//   Texturing & Modeling - A Procedural Approach 3rd Ed.
//   Ebert, Musgrave, Peachy, Perlin and Worley
//   ISBN: 1-55860-848-6
//
//   Mathematics for 3D Game Programming & Computer Graphics 2nd Ed.
//   Eric Lengyel
//   ISBN: 1-58450-277-0
//
//   Inspiration from:
//   http://rgba.scenes$p->org/iq/computer/articles/terrainmarching/terrainmarching.htm
//   Thanks iigo!
//

class LandscapeGenerator {

	/** @var float[3] */
	public $cameraPosition = array(-6, 4, 6);
	
	/** @var float[3] */
	public $cameraPositionFine = array(0, 0, 0);
	
	/** @var float */
	public $cameraRotation = 0;
	
	/** @var float */
	public $terrainHorizon = 25;
	
	/** @var float */
	public $terrainDetail = 7;
	
	/** @var bool */
	public $buildings = false;
	
	/** @var float */
	public $waterLevel = 0.34;
	
	/** @var bool */
	public $shadows = true;
	
	/** @var bool */
	public $contours = false;
	
	/** @var float[x,y] */
	public $sun = array(14.0, 252.0);
	
	/** @var float */
	public $terrainHeight = 3.5;

	/** @var float */
	protected $detailLevel;
	
	/** @var float */
	protected $aspectRatio;
	
	/** @var float */
	protected $ambient;
	
	/** @var float */
	protected $specular;
	
	/** @var float */
	protected $sunSize;
	
	/** @var float */
	protected $contourInterval;
	
	/** @var float[x,y,z] */
	protected $sunPosition;
	
	/** @var float[x,y,z] */
	protected $cameraEye;

	/** @var float[3][3] */
	protected $viewRotation;
	
	/** @var float[3] */
	protected $fogColor;

	/** @var float[3] */
	protected $valleyColor;

	/** @var float[3] */
	protected $peakColor;
	
	/** @var float[3] */
	protected $buildingColor;
	
	/** @var float[3] */
	protected $waterColor;
	
	/** @var float[3] */
	protected $skyColor;
	
	/** @var float[3] */
	protected $sunColor;
	
	/** @var float[1024] */
	protected $start_dst;
	
	/** @var float */
	protected $width;

	/** @var float */
	protected $height;
	
	const EPSILON			= 0.1;		// The initial ray marching step size
	const SPECULAR_EXPONENT	= 50.0;		// The water specular component
	
	/**
	 * Find the terrain displacement for the current point
	 * 
	 * return[0] = distance to surface
	 * return[1] = depth of water or height of building
	 * return[2] = surface type; 0 = hills, 1 = water, 2 = buildings
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$p
	 *
	 * @local float					$(alt|hills|structures)
	 * @local float[3]				$terrain
	 *
	 * @return float[3]
	 */
	protected function displacement($p)
	{
		$hills = abs(2.0 * cos($p->x/32.0 - 0.5) * sin($p->z/32.0 - 0.5) + 
						   cos($p->x/8.0) * sin($p->z/8.0 + 0.5) + 
						   cos($p->x/2.0 - 0.5) * sin($p->z/2.0 + 0.5) + 
						   cos(2.0 * $p->x) * sin(2.0 * $p->z) / 8.0) / 2.125;
		
		if ($hills <= $this->waterLevel)
		{
			// Hit water
			$terrain = array($this->terrainHeight * $this->waterLevel, $this->terrainHeight * $hills, 1);
		}
		else if ($this->buildings)
		{
			// Only show buildings over a certain threshold
			$structures = $this->waterLevel + (mod($p->x / 2.0, 0.8) - mod($p->x / 2.0, 0.5)) * 
									  (mod($p->z / 2.0, 0.8) - mod($p->z / 2.0, 0.5)) * $this->terrainHeight / 4.0;
			
			if ($structures < $hills)
			{
				$terrain = array($this->terrainHeight * $hills, 0, 0);
			} else {
				$terrain = array($this->terrainHeight * $structures, $structures, 2);
			}
		} else {
			$terrain = array($this->terrainHeight * $hills, 0, 0);
		}
		
		return $terrain;
	}
	
	
	/**
	 * Define the ray direction from the pixel coordinates
	 * 
	 * @engine qb
	 *
	 * @param float[x,y]			$p
	 *
	 * @local float[x,y,z]			$direction
	 *
	 * @return float[3]
	 */
	protected function rayDirection($p)
	{
		$direction->x = 2.0 * $this->aspectRatio * $p->x / $this->width - $this->aspectRatio;
		$direction->y = -2.0 * $p->y / $this->height + 1.0;
		$direction->z = 2.0;
		return vm_mult_cm($direction, $this->viewRotation);
	}
	
	
	/**
	 * Find the normal vector at the intersection of the ray and terrain
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$p
	 * @param float					$epsilon
	 *
	 * @local float[3]				$(p1|p2|p3|p4)
	 * @local float[3]				$(d1|d2|d3|d4)
	 * @local float[x,y,z]			$n
	 *
	 * @return float[3]
	 */
	protected function intersectionNormal($p, $epsilon)
	{
		$p1 = array($p->x - $epsilon, $p->y, $p->z);
		$p2 = array($p->x + $epsilon, $p->y, $p->z);
		$p3 = array($p->x,	$p->y, $p->z - $epsilon);
		$p4 = array($p->x,	$p->y, $p->z + $epsilon);
		
		$d1 = $this->displacement($p1);
		$d2 = $this->displacement($p2);
		$d3 = $this->displacement($p3);
		$d4 = $this->displacement($p4);
		
		$n->x = $d1[0] - $d2[0];
		$n->y = 2.0 * $epsilon;
		$n->z = $d3[0] - $d4[0];
		return normalize($n);
	}
	
	
	/**
	 * Trace the ray through the scene
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$ray_direction
	 * @param float					$t
	 * @param float[x,y,z]			$ray
	 * @param float[x,y,z]			$normal
	 * @param float					$ray_distance
	 * @param float[3]				$surface
	 *
	 * @local float					$(dt|start_d|prev_alt|id)
	 * @local float[x,y,z]			$(basic_ray|prev_ray)
	 *
	 * @return void
	 */
	protected function castRay($ray_direction, $t, &$ray, &$normal, &$ray_distance, &$surface)
	{
		$dt = self::EPSILON;				 // Step size
		$ray = array(0.0, 0.0, 0.0);
		$start_d = $t;
		
		// March ray until we reach the horizon
		while ($t < $this->terrainHorizon)
		{
			$ray_distance = -1.0;			// < 0 when no intersection
			
			// Vector for current position
			$ray = $this->cameraEye + $ray_direction * $t;
			
			// If greater than the max terrain height and moving away from the plane then stop
			if ($ray->y > $this->terrainHeight * 2.0 && $ray->y > $prev_ray->y && $t > $start_d)
				break;

			// Find terrain displacement for the current point
			$surface = $this->displacement($ray);
			
			if ($ray->y <= $surface[0])
			{
				// Interpolate the intersection distance
				$id = ($prev_alt - $prev_ray->y)/($ray->y -  $prev_ray->y - $surface[0] + $prev_alt);
				$ray_distance = mix($t, ($t-$dt), $id);
				$surface[0] = mix($surface[0], $ray->y, $id);
				$ray->y = $surface[0];
				$prev_ray->y = $prev_alt;
				$normal = $this->intersectionNormal($ray, $dt);
				break;
			}
			
			// Step size increases proportionally to the ray length
			$t += $dt;
			$dt = max(($t - $start_d) * $this->detailLevel, 0.005);
			$prev_alt = $surface[0];
			$prev_ray = $ray;
		}
	}
	
	
	/**
	 * Trace a ray from the origin point in the terrain coordinates to the sun. 
	 * If it intersects anything then we are in shadow.
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$origin
	 * @param float[x,y,z]			$direction
	 *
	 * @local float[3]				$surface
	 * @local float[x,y,z]			$ray
	 * @local float					$shadow_length
	 * @local float					$t
	 *
	 * @return float
	 */
	protected function castShadowRay($origin, $direction)
	{
		$t = self::EPSILON;
		$surface = $this->displacement($origin);
		$origin->y = $surface[0];
		
		// Trace ray until we intersect terrain or reach the terrain ceiling
		while ($ray->y < $this->terrainHeight)
		{
			// Vector for current position
			$ray = $origin + $direction * $t;
			$surface = $this->displacement($ray);
			
			if ($ray->y <= $surface[0])
			{
				$shadow_length = $t;
				break;
			}
			$t += self::EPSILON;
		}
		return $shadow_length;
	}
	
	
	/**
	 * Add a fog factor depending on the distance away from the camera and terrain horizon
	 *
	 * @engine qb
	 *
	 * @param float[3]				$c
	 * @param float					$d
	 *
	 * @local float					$r
	 *
	 * @return float[3]
	 */
	protected function fog($c, $d)
	{
		$r = abs($d) / $this->terrainHorizon;
		return mix($c, $this->fogColor, $r);
	}
	
	
	/**
	 * Add shadow factor depending on the shadow length
	 *
	 * @engine qb
	 *
	 * @param float					$diffuse
	 * @param float					$l
	 *
	 * @return float
	 */
	protected function shadow($diffuse, $l)
	{
		if ($l > 0.0)
		{
			$diffuse = clamp($diffuse - 0.01, 0.0, 1.0) * $this->ambient;
		}
		return $diffuse;
	}
	
	
	/**
	 * Sky colour
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$ray_direction
	 *
	 * @local float					$a
	 * @local float[3]				$s
	 *
	 * @return float[3]
	 */
	protected function sky($ray_direction)
	{
		$a = clamp((abs(acos(dot($ray_direction, $this->sunPosition) / (length($ray_direction) * length($this->sunPosition)))) / 3.14) / $this->sunSize, 0.0, 1.0) ;
		$s = $this->sunColor * (1.0 - $a);
		
		return $this->skyColor * $this->ambient + $s;
	}
	
	
	/**
	 * Contours
	 *
	 * @engine qb
	 *
	 * @param float[x,y,z]			$o
	 * @param float					$h
	 *
	 * @local float					$contour
	 *
	 * @return bool
	 */
	protected function overlayContour($o, $h)
	{
		$contour = mod($h * 100.0, $this->contourInterval) - mod(($h * 100.0 - 1.5), $this->contourInterval);
		
		return ($this->contours && $contour < 0.5 ? true : false);
	}
	
	
	/**
	 * The main render function returns a colour for the input pixel coordinates
	 *
	 * @engine qb
	 *
	 * @param float[x,y]			$p
	 *
	 * @local float[3]				$(o|surface)
	 * @local float[x,y,z]			$(ray|l|reflection|ray_direction|n)
	 * @local float					$(ray_distance|diffuse|shadow_length|t|contour|cf|spec)
	 *
	 * @return float[3]
	 */
	protected function render($p)
	{
		$t = $this->start_dst[$p->x / 5.0];
		$ray_direction = $this->rayDirection($p);
		
		$this->castRay($ray_direction, $t, $ray, $n, $ray_distance, $surface);
		
		if ($ray_distance >= 0.0)
		{
			if ($surface[2] == 1.0)
			{
				// Water level
				$l = normalize($this->sunPosition - $ray);
				$diffuse = dot($l, $n);
				$o = $this->waterColor;
				
				if ($this->shadows)
				{
					$shadow_length = $this->castShadowRay($ray, $l);
					$diffuse = $this->shadow($diffuse, $shadow_length);
				}
				
				if ($shadow_length == 0.0)
				{
					// Coast factor
					$cf = ($surface[0] - $surface[1]) / ($this->waterLevel * $this->terrainHeight);
					if ($cf < 0.35)
						$diffuse *= 1.35 - $cf;
				
					$reflection = $l - 2.0 * $diffuse * $n;		 // reflect the light vector 
					$spec = dot(normalize($ray), $reflection);
					$spec = pow(max($this->specular, 0.0), $this->specular) * 0.5;
					$o = $o * $diffuse * (0.5 + $spec);
					if ($this->overlayContour($o, $surface[1]))
						$o *= 1.4;
					
					$o = $this->fog($o, $ray_distance);
				} else {
					$diffuse *= 0.4;
					if ($this->overlayContour($o, $surface[0]))
						$o *= 1.4;
					$o = $this->fog($o * $diffuse, $ray_distance);
				}
			} else if ($surface[2] == 2.0) {
				// Building
				$l = normalize($this->sunPosition - $ray);
				$diffuse = 0.3 + dot($l, $n) / 1.43;
				
				if ($this->shadows)
				{
					$shadow_length = $this->castShadowRay($ray, $l);
					$diffuse = $this->shadow($diffuse, $shadow_length);
				}
				
				$o = $this->buildingColor * $diffuse;
				$o = $this->fog($o, $ray_distance);
				
			} else {
				// Landscape colour
				$l = normalize($this->sunPosition - $ray);
				$diffuse = 0.3 + dot($l, $n) / 1.43;
				
				if ($this->shadows)
				{
					$shadow_length = $this->castShadowRay($ray, $l);
					$diffuse = $this->shadow($diffuse, $shadow_length);
				}
				
				$o = mix($this->valleyColor, $this->peakColor, clamp(($ray->y - $this->waterLevel) / ($this->terrainHeight - $this->terrainHeight * $this->waterLevel), 0.0, 1.0)) * $diffuse;
				
				if ($this->overlayContour($o, $surface[0]))
					$o *= 0.4;
				
				$o = $this->fog($o, $ray_distance);
			}
		} else {
			// Sky colour
			$o = mix($this->sky($ray_direction), $this->fogColor, 1.0 - ($this->height / 1.8 - $p->y) / $this->height);
		}
		
		return $o;
	}
	
	
	/**
	 * The main loop
	 *
	 * @engine qb
	 *
	 * @param image3				$dst
	 *
	 * @local uint					$(width|height)
	 * @local float[x,y]			$coord
	 * @local uint					$(x|y|i|j)
	 * @local float					$(c|s|sd)
	 * @local float[x,y,z]			$(ray|n|ray_direction)
	 * @local float[3]				$surface
	 * @local float[3]				$pixel
	 * @local float					$(ray_distance|h)
	 *
	 */
	public function generate(&$dst) 
	{
		$height = count($dst);
		$width = count($dst[0]);
		
		$this->width = $width;
		$this->height = $height;
		$this->aspectRatio = $this->width / $this->height;
	
		// Camera orientation
		$c = cos(deg2rad(-$this->cameraRotation));
		$s = sin(deg2rad(-$this->cameraRotation));
		$this->viewRotation = array( $c, 0, $s, 0, 1, 0, -$s, 0, $c );
		
		// Set up camera eye
		$this->cameraEye = $this->cameraPosition + $this->cameraPositionFine;
		
		if ($this->cameraEye->y <= ($this->waterLevel * $this->terrainHeight))
			$this->cameraEye->y = ($this->waterLevel * $this->terrainHeight) + 0.1;
		
		// Sun position
		$sd = 1000.0;	// Sun distance from plane
		$this->sunPosition = array($sd * cos(deg2rad($this->sun->y)) * sin(deg2rad(90.0 - $this->sun->x)),
									$sd * cos(deg2rad(90.0 - $this->sun->x)),
							 		$sd * sin(deg2rad($this->sun->y)) * sin(deg2rad(90.0 - $this->sun->x)));
		$this->sunPosition += $this->cameraEye;
		
		// Others
		$this->detailLevel = (11.0 - $this->terrainDetail) / 175.0;
		$this->specular = 0.5;
		$this->sunSize = 0.17;
		$this->contourInterval = 10.0;
		
		// Find the starting distance for current view
		// Sample points from the bottom row and use the smallest distance as the
		// ray marching starting distance
		
		// Collision check
		for ($x = 0.0; $x < $this->width; $x += $this->width / 2.0)
		{
			$ray = $this->cameraEye + $this->rayDirection(array($x, $this->height)) * 0.1;
			$surface = $this->displacement($ray);
			
			if ($ray->y < $surface[0])
			{
				$ray_direction = $this->rayDirection(array($x, $this->height));
				$this->cameraEye->y = $surface[0] - $ray_direction->y * 0.1 + self::EPSILON;
			}
		}
		
		// Find start positions for the bottom row - it's an extra row of calcuations but should give a better result
		for ($i = 0.0; $i < $this->width; $i += 5.0)
		{
			$this->castRay($this->rayDirection(array($i, $this->height)), 0.01, $ray, $n, $ray_distance, $surface);
			$ray_distance -= self::EPSILON;
			if ($ray_distance < self::EPSILON) $ray_distance = self::EPSILON;
			$this->start_dst[$i / 5.0] = $ray_distance;
		}
		
		// Colours
		$this->skyColor	  = array(0.0, 0.0, 1.0);
		$this->sunColor	  = array(1.0, 1.0, 1.0);
		$this->fogColor	  = array(251.0, 251.0, 253.0) / 255.0;
		$this->waterColor	= array(0.0, 0.3, 1.0);
		$this->valleyColor   = array(75.0, 139.0, 44.0) / 255.0;
		$this->peakColor	 = array(189.0, 178.0, 149.0) / 255.0;
		$this->buildingColor = array(203.0, 199.0, 188.0) / 255.0;
		$this->ambient	   = clamp($this->sun->x / 50.0, 0.5, 1.0);
		
		$y = fork($height);
		$coord->y = $y + 0.5;

		for ($x = 0, $coord->x = 0.5; $x < $width; $x++, $coord->x++) {
			$pixel = $this->render($coord);
			$dst[$y][$x] = $pixel;
		}
	}
}

// so the script wouldn't slow to a grind if xdebug is active
ini_set("qb.allow_debugger_inspection", 0);

ini_set("qb.column_major_matrix", 1);

$folder = dirname(__FILE__);
$image = imagecreatefrompng("$folder/input/ipad.png");
$output = imagecreatetruecolor(400, 200);
$correct_path = "$folder/output/landscape.correct.png";
$incorrect_path = "$folder/output/landscape.fork.incorrect.png";

$generator = new LandscapeGenerator;
$generator->cameraRotation = 130;
$generator->buildings = true;
$generator->generate($output);

ob_start();
imagepng($output);
$output_png = ob_get_clean();

/**
 * @engine qb
 *
 * @param image	$img2;
 * @param image	$img1;
 * @return float32
 */
function _image_diff($img1, $img2) {
	$img2 -= $img1;
	$img2 *= $img2;
	return sqrt(array_sum($img2));
}

if(file_exists($correct_path)) {
	$correct_md5 = md5_file($correct_path);
	$output_md5 = md5($output_png);
	if($correct_md5 == $output_md5) {
		// exact match
		$match = true;
	} else {
		$correct_output = imagecreatefrompng($correct_path);
		$diff = _image_diff($output, $correct_output);
		if($diff < 3) {
			// the output is different ever so slightly
			$match = true;
		} else {
			$match = false;
		}
		//$match = false;
	}
	if($match) {
		echo "CORRECT\n";
		if(file_exists($incorrect_path)) {
			unlink($incorrect_path);
		}
	} else {
		echo "INCORRECT (diff = $diff)\n";
		file_put_contents($incorrect_path, $output_png);
	}
} else {
	// reference image not yet available--save image and inspect it for correctness by eye
	file_put_contents($correct_path, $output_png);
	echo "CORRECT\n";
}


?>
--EXPECT--
CORRECT
