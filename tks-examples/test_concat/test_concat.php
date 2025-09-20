<?php

function microtime_float()
{
   list($usec, $sec) = explode(" ", microtime());
   return ((float)$usec + (float)$sec);
}

$t = microtime_float();

$s1 = "hallo, ";
$s2 = "welt...";
$s3 = "some ";
$s4 = "string";
$s5 = "concatenations";

$r = "";

$l = 0;

for($i=0; $i<10000000; $i++)
{
   $r = $s1 . $s2 . $s3 . $s4 . $s5;
   $l = strlen($r);
}

echo "l=" .$l ."\n";
echo "ms = " . ((microtime_float()-$t)*1000) . "\n";

?>