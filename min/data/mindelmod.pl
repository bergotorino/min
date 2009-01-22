#!/usr/bin/perl
# Searches for a Module with item give in second argument (usually ModuleName=)
# from file given as first argument. Removes the whole section if found.
#

use strict;
use File::Copy;

my $file = $ARGV[0];
my $modulename = $ARGV[1];
my @arr;
my $flag;

copy( $file, "$file.backup" ) or die "Copy failed: $!";

open my $in,  '<',  "$file.backup"  or die "Can't read input file: $!";
open my $out, '>', $file or die "Can't write to output file: $!";

while( <$in> ) {
        if (/^\[New_Module\]/) {
	    $flag = 0;
	    while (<$in>) {
		if (/$modulename/) {
	            $flag = 1;
		} else {
		    push @arr, $_;
	        }
		last if (/^\[End_Module\]/);
	    }
	    if ($flag == 0) {
		print $out "[New_Module]\n";
		while (<@arr>) {
		    print $out "$_\n";
		    shift @arr;
		}
		print $out "[End_Module]\n";
	    } else {
		while (pop @arr) {;}
	    }
	}
        else {     
             print $out $_;
        }
}
print $out "\n";

close $out;

