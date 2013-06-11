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
		debug_print_backtrace();
		die("No implementation for $className");
	}
	$classPath = $classPaths[$className];
    include $classPath;
}

spl_autoload_register('autoload');

error_reporting(E_ALL | E_STRICT);
set_time_limit(0);

$copyright = file_get_contents("$scriptFolder/copyright.txt");
$generator = new QBCodeGenerator;

echo "Creating qb_interpreter_gcc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_gcc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'GCC', 'DECLARATIONS');
$generator->generate($handle, 'GCC', 'HANDLERS');
$generator->generate($handle, 'GCC', 'NATIVE SYMBOLS');
$generator->generate($handle, 'GCC', 'NATIVE DEBUG');

echo "Creating qb_interpreter_llvm.c\n";
$handle = fopen("$targetFolder/qb_interpreter_llvm.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'GCC', 'DECLARATIONS');
$generator->generate($handle, 'GCC', 'HANDLERS');
$generator->generate($handle, 'GCC', 'NATIVE SYMBOLS');
$generator->generate($handle, 'GCC', 'NATIVE DEBUG');

echo "Creating qb_interpreter_msvc.c\n";
$handle = fopen("$targetFolder/qb_interpreter_msvc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'MSVC', 'DECLARATIONS');
$generator->generate($handle, 'MSVC', 'HANDLERS');
$generator->generate($handle, 'MSVC', 'NATIVE SYMBOLS');
$generator->generate($handle, 'MSVC', 'NATIVE DEBUG');

echo "Creating qb_opcodes.h\n";
$handle = fopen("$targetFolder/qb_opcodes.h", "w");
fwrite($handle, $copyright);
$generator->generate($handle, 'GCC', 'CODES');

echo "Creating qb_data_tables_gcc.c\n";
$handle = fopen("$targetFolder/qb_data_tables_gcc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'GCC', 'FLAGS');
$generator->generate($handle, 'GCC', 'NAMES');
$generator->generate($handle, 'GCC', 'NATIVE CODE');

echo "Creating qb_data_tables_llvm.c\n";
$handle = fopen("$targetFolder/qb_data_tables_llvm.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'LLVM', 'FLAGS');
$generator->generate($handle, 'LLVM', 'NAMES');
$generator->generate($handle, 'LLVM', 'NATIVE CODE');

echo "Creating qb_data_tables_msvc.c\n";
$handle = fopen("$targetFolder/qb_data_tables_msvc.c", "w");
fwrite($handle, $copyright);
fwrite($handle, "#include \"qb.h\"\n\n");
$generator->generate($handle, 'MSVC', 'FLAGS');
$generator->generate($handle, 'MSVC', 'NAMES');
$generator->generate($handle, 'MSVC', 'NATIVE CODE');

?>