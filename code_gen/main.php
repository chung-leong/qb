<?php

set_time_limit(30);
ob_implicit_flush(true);

$scriptFolder = dirname(__FILE__);
$targetFolder = dirname($scriptFolder);

$classPaths = array();
foreach(array_merge(glob("$scriptFolder/*.php"), glob("$scriptFolder/*/*.php"), glob("$scriptFolder/*/*/*.php")) as $classPath) {
	$className = substr($classPath, strrpos($classPath, '/') + 1, -4); 
	$classPaths[$className] = $classPath;
}

function autoload($className) {
	global $classPaths;
	if(!$classPaths[$className]) {
		debug_print_backtrace();
		die("No implementation for $className");
	}
	$classPath = $classPaths[$className];
    include $classPath;
}

spl_autoload_register('autoload');

function create_file($filename) {
	global $targetFolder, $fileHandles;

	echo "Creating $filename\n";
	$path = "$targetFolder/$filename";
	$handle = fopen($path, "w+");
	$fileHandles[] = $handle;
	return $handle;
}

function fix_line_endings() {
	global $fileHandles;
	foreach($fileHandles as $handle) {
		fseek($handle, 0, SEEK_END);
		$size = ftell($handle);
		fseek($handle, 0, SEEK_SET);
		$data = fread($handle, $size);
		$data = str_replace("\r\n", "\n", $data);
		if(PHP_EOL != "\n") {
			$data = str_replace("\n", PHP_EOL, $data);
		}
		ftruncate($handle, 0);
		fseek($handle, 0, SEEK_SET);
		fwrite($handle, $data);
		fclose($handle);
	}
}

error_reporting(E_ALL | E_STRICT);
set_time_limit(0);

$copyright = file_get_contents("$scriptFolder/listings/copyright.txt");
$include = "#include \"qb.h\"\n";

$generator = new CodeGenerator;

$handle = create_file("qb_interpreter_gcc.h");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeTypeDeclarations($handle, 'GCC');
$generator->writeFunctionPrototypes($handle, 'GCC');

$handle = create_file("qb_interpreter_msvc.h");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeTypeDeclarations($handle, 'MSVC');
$generator->writeFunctionPrototypes($handle, 'MSVC');

$handle = create_file("qb_interpreter_loop_gcc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_gcc.h\"\n");
fwrite($handle, "\n");
$generator->writeMainLoop($handle, 'GCC');
$generator->writeNativeDebugStub($handle, 'GCC');

$handle = create_file("qb_interpreter_loop_msvc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_msvc.h\"\n");
fwrite($handle, "\n");
$generator->writeMainLoop($handle, 'MSVC');
$generator->writeNativeDebugStub($handle, 'MSVC');

$handle = create_file("qb_interpreter_functions_gcc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_gcc.h\"\n");
fwrite($handle, "\n");
$generator->writeFunctionDefinitions($handle, 'GCC');
$generator->writeNativeSymbolTable($handle, 'GCC');

$handle = create_file("qb_interpreter_functions_msvc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_msvc.h\"\n");
fwrite($handle, "\n");
$generator->writeFunctionDefinitions($handle, 'MSVC', 'FUNCTION DEFINITIONS');
$generator->writeNativeSymbolTable($handle, 'MSVC', 'NATIVE SYMBOLS');

$handle = create_file("qb_opcodes.h");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeOpCodes($handle);

$handle = create_file("qb_data_tables_gcc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_gcc.h\"\n");
fwrite($handle, "\n");
$generator->writeOpInfo($handle);
$generator->writeOpNames($handle);
$generator->writeNativeCodeTables($handle, 'GCC');

$handle = create_file("qb_data_tables_msvc.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_msvc.h\"\n");
fwrite($handle, "\n");
$generator->writeOpInfo($handle, 'MSVC');
$generator->writeOpNames($handle, 'MSVC');
$generator->writeNativeCodeTables($handle, 'MSVC');

fix_line_endings();

?>
