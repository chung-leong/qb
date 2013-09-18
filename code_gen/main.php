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

error_reporting(E_ALL | E_STRICT);
set_time_limit(0);

$copyright = file_get_contents("$scriptFolder/listings/copyright.txt");
$include = <<<INCLUDE
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "qb.h"

INCLUDE;

$generator = new CodeGenerator;

echo "Creating qb_interpreter_gcc.h\n";
$handle = fopen("$targetFolder/qb_interpreter_gcc.h", "w");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeTypeDeclarations($handle, 'GCC');
$generator->writeFunctionPrototypes($handle, 'GCC');

echo "Creating qb_interpreter_msvc.h\n";
$handle = fopen("$targetFolder/qb_interpreter_msvc.h", "w");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeTypeDeclarations($handle, 'MSVC');
$generator->writeFunctionPrototypes($handle, 'MSVC');

echo "Creating qb_interpreter_gcc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_gcc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_gcc.h\"\n");
fwrite($handle, "\n");
$generator->writeMainLoop($handle, 'GCC');
$generator->writeNativeDebugStub($handle, 'GCC');

echo "Creating qb_interpreter_msvc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_msvc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_msvc.h\"\n");
fwrite($handle, "\n");
$generator->writeMainLoop($handle, 'MSVC');
$generator->writeNativeDebugStub($handle, 'MSVC');

echo "Creating qb_interpreter_functions_gcc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_functions_gcc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_gcc.h\"\n");
fwrite($handle, "\n");
$generator->writeFunctionDefinitions($handle, 'GCC');
$generator->writeNativeSymbolTable($handle, 'GCC');

echo "Creating qb_interpreter_functions_msvc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_functions_msvc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "#include \"qb_interpreter_msvc.h\"\n");
fwrite($handle, "\n");
$generator->writeFunctionDefinitions($handle, 'MSVC', 'FUNCTION DEFINITIONS');
$generator->writeNativeSymbolTable($handle, 'MSVC', 'NATIVE SYMBOLS');

echo "Creating qb_opcodes.h\n";
$handle = fopen("$targetFolder/qb_opcodes.h", "w");
fwrite($handle, $copyright);
fwrite($handle, "\n");
$generator->writeOpCodes($handle);

echo "Creating qb_data_tables_gcc.c\n";
$handle = fopen("$targetFolder/qb_data_tables_gcc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "\n");
$generator->writeOpInfo($handle);
$generator->writeOpNames($handle);
$generator->writeNativeCodeTables($handle, 'GCC');

echo "Creating qb_data_tables_msvc.c\n";
$handle = fopen("$targetFolder/qb_data_tables_msvc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, $include);
fwrite($handle, "\n");
$generator->writeOpInfo($handle, 'MSVC');
$generator->writeOpNames($handle, 'MSVC');
$generator->writeNativeCodeTables($handle, 'MSVC');

?>