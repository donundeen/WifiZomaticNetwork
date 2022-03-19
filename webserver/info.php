<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
$output=null;
$retval=null;
exec("/usr/bin/aplay /home/pi/WifiZomaticNetwork/audio/fs1.wav 2>&1", $output, $retval);
//exec ("touch /home/pi/writehere/touch.txt"); 

echo "Returned with status $retval and output:\n";
print_r($output);

phpinfo();

?>
