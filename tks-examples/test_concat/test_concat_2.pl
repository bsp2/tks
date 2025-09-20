#/usr/bin/perl

use Time::HiRes qw(gettimeofday);

($epochseconds, $microseconds) = gettimeofday; 

$ms = ($epochseconds*1000) + ($microseconds/1000);
$t = $ms;

$s1 = "hallo, ";
$s2 = "welt...";
$s3 = "some ";
$s4 = "string";
$s5 = "concatenations";

$l = 0;

for($i=0; $i<10000000; $i++)
{
    $r = $s1;
    $r = $r . $s2;
    $r = $r . $s3;
    $r = $r . $s4;
    $r = $r . $s5;
    $l = length($r);
}

($epochseconds, $microseconds) = gettimeofday; 
$ms = ($epochseconds*1000) + ($microseconds/1000);
print "l=".$l."\n";
print "ms = " . ($ms-$t) . "\n";
