--TEST--
phpinfo() test
--FILE--
<?php

ob_start();
phpinfo(INFO_MODULES);
$text = ob_get_clean();
$lines = preg_split('/[\r\n]+/', $text); 

foreach($lines as $line) {
	if(preg_match('/qb support/', $line)) {
		echo $line, "\n";
	}
}

?>
--EXPECT--
qb support => enabled

