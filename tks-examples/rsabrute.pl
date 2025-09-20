#
# aus c't 19/03
#

# RealCalcLoop
#my $itime=0;
my $Res=0;
my $x=1;
for($x=1; $x<=100; $x++)
{
    my $y=1;
    for($y=1; $y<=1000; $y++)
    {
	my $z=1;
	for($z=1; $z<=1000; $z++)
	{
	    $Res+=($x+$y+$z) % 4;
	}
    }
}


