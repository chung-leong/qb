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
$include = '
#include "qb.h"
#include "qb_interpreter_structures.h"
#include "qb_interpreter_functions.h"
';

$generator = new CodeGenerator;

$handle = create_file("qb_opcodes.h");
fwrite($handle, $copyright);
fwrite($handle, "\n\n");
fwrite($handle, "#ifndef QB_OPCODES_H_\n");
fwrite($handle, "#define QB_OPCODES_H_\n");
fwrite($handle, "\n");
$generator->writeOpCodes($handle);
fwrite($handle, "#endif\n");

$handle = create_file("qb_interpreter_structures.h");
fwrite($handle, $copyright);
fwrite($handle, "\n\n");
fwrite($handle, "#ifndef QB_INTERPRETER_STRUCTURES_H_\n");
fwrite($handle, "#define QB_INTERPRETER_STRUCTURES_H_\n");
fwrite($handle, "\n");
$generator->writeTypeDeclarations($handle);
fwrite($handle, "#endif\n");

$handle = create_file("qb_interpreter_functions.h");
fwrite($handle, $copyright);
fwrite($handle, "\n\n");
fwrite($handle, "#ifndef QB_INTERPRETER_FUNCTIONS_H_\n");
fwrite($handle, "#define QB_INTERPRETER_FUNCTIONS_H_\n");
fwrite($handle, "\n");
$generator->writeFunctionPrototypes($handle);
fwrite($handle, "#endif\n");

$handle = create_file("qb_interpreter_loop.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "\n");
$generator->writeMainLoop($handle);
$generator->writeNativeDebugStub($handle);

$handle = create_file("qb_interpreter_functions.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "\n");
$generator->writeFunctionDefinitions($handle);
$generator->writeNativeSymbolTable($handle);

$handle = create_file("qb_data_tables.c");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "\n");
$generator->writeOpInfo($handle);
$generator->writeOpNames($handle);
$generator->writeNativeCodeTables($handle);

fix_line_endings();

?>
