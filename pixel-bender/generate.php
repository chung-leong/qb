<?php

//$info = qb_extract($filterPath, QB_PBJ_DETAILS);

$source = $_GET['source'];
$kernel = $_GET['kernel'];

// get the kernel's function declaration and declare the function
$decl = qb_extract($kernel, QB_PBJ_DECLARATION);
$info = qb_extract($kernel, QB_PBJ_DETAILS);
eval($decl);

// compile it
qb_compile(QB_SCAN_ALL);

// load the source image and create the output image
$data = file_get_contents($source);
$image = imagecreatefromstring($data);
$output = imagecreatetruecolor(imagesx($image), imagesy($image));

// load the texture if one is required
if(isset($_GET['texture'])) {
	$texture = $_GET['texture'];
	$data = file_get_contents($texture);
	$bitmap = imagecreatefromstring($data);
}

// build the argument list
$arguments = array(&$output, $image);
if(isset($bitmap)) {
	$arguments[] = $bitmap;
}
$parameters = $info['parameters'];
foreach($parameters as $parameter) {
	if($parameter['direction'] == 'in' && !preg_match('/image/', $parameter['type'])) {
		$name = $parameter['name'];
		$value = null;
		$string = isset($_GET[$name]) ? $_GET[$name] : 0;
		if($string) {
			
			switch($parameter['type']) {
				case 'bool':
				case 'int':
					$value = intval($string);
					break;
				case 'bool2':
				case 'bool3':
				case 'bool4':
				case 'int2':
				case 'int3':
				case 'int4':
					$value = array_map('intval', explode(',', $string));
					break;
				case 'float':
					$value = floatval($string);
					break;
				case 'float2':
				case 'float3':
				case 'float4':
				case 'float2x2':
				case 'float3x3':
				case 'float4x4':
					$value = array_map('floatval', explode(',', $string));
					break;
			}
		} else {
			$value = $parameter['defaultValue'];
		}
		$arguments[] = $value;
	}
}

// call the function
call_user_func_array($info['name'], $arguments);

imagedestroy($image);

$final_image = imagecreatetruecolor(imagesx($output), imagesy($output));
imagefilledrectangle($final_image, 0, 0, imagesx($output), imagesy($output), imagecolorallocate($final_image, 0xFF, 0xFF, 0xFF));
imagecopy($final_image, $output, 0, 0, 0, 0, imagesx($output), imagesy($output));

// send the output
header("Content-type: image/jpeg");

imagejpeg($final_image);

?>