
my $i=0;
for($j=0; $j<1000; $j++)
{
    for($k=0; $k<1000; $k++)
    {
	my $s="one two three four_and_a_half";
	my $t;
	foreach $t (split(/ /,$s))
	{
	    $i++;
	}
    }
}
print "i=$i\n";
