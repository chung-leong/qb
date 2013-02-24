<?php

set_time_limit(30);
ob_implicit_flush(true);

$scriptFolder = dirname(__FILE__);
$targetFolder = dirname($scriptFolder);

$classPaths = array();
foreach(array_merge(glob("$scriptFolder/QB*.php"), glob("$scriptFolder/*/QB*.php")) as $classPath) {
	$className = substr($classPath, strrpos($classPath, '/') + 1, -4); 
	$classPaths[$className] = $classPath;
}

function autoload($className) {
	global $classPaths;
	if(!$classPaths[$className]) {
		die("No implementation for $className");
	}
	$classPath = $classPaths[$className];
    include $classPath;
}

spl_autoload_register('autoload');

error_reporting(E_ALL | E_STRICT);

$generator = new QBCodeGenerator;

echo "Creating qb_interpreter_gcc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_gcc.c", "w");
$generator->generate($handle, 'GCC', 'DECLARATIONS');
$generator->generate($handle, 'GCC', 'HANDLERS');

echo "Creating qb_interpreter_msvc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_msvc.c", "w");
$generator->generate($handle, 'MSVC', 'DECLARATIONS');
$generator->generate($handle, 'MSVC', 'HANDLERS');

echo "Creating qb_opcodes.h\n";
$handle = fopen("$targetFolder/qb_opcodes.h", "w");
$generator->generate($handle, 'GCC', 'CODES');

echo "Creating qb_data_tables.c\n";
$handle = fopen("$targetFolder/qb_data_tables.c", "w");
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'GCC', 'FLAGS');
$generator->generate($handle, 'GCC', 'NAMES');
$generator->generate($handle, 'GCC', 'NATIVE CODE');

echo "Creating qb_interpreter_symbols.c\n";
$handle = fopen("$targetFolder/qb_interpreter_symbols.c", "w");
$generator->generate($handle, 'GCC', 'NATIVE SYMBOLS');

?>