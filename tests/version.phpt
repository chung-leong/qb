--TEST--
Version test
--FILE--
<?php

echo phpversion("qb"), "\n";

?>
--EXPECTREGEX--
2\.\d\.\d+.*

