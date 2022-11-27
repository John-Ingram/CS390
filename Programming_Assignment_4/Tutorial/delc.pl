#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;

use feature 'say';
use feature "switch";

use v5.16;


my @file_list;

if (@ARGV) 
{    
    @file_list = @ARGV; 

    # if the first argument is a splat, then we need to expand it
    if ($file_list[0] eq '*') 
    {
        @file_list = glob '*';
    }
} 
else 
{     
    say("ERROR: no args\n");     
    exit 1; 
}  
# if we get here, we have a list of files to process
# Turn the list of files into a hash of file names and undef values
my %files_hash = map { $_ => undef } @file_list;

my $file_Will_Be_Deleted = 'n';

foreach (keys %files_hash) 
{  
    # ask the user if they want to delete the file, and store the answer in the hash
    say("Delete $_? (y/n)");
    $files_hash{$_} = <STDIN>;
    chomp $files_hash{$_};

    # if the user wants to delete the file, then set the flag to 'y'
    if ($files_hash{$_} eq 'y') 
    {
        $file_Will_Be_Deleted = 'y';
    }
} 

# if the user dosn't want to delete any files, exit
if ($file_Will_Be_Deleted eq 'n') 
{
    say("No files will be deleted");
    exit 0;
}

# now we have a hash of files and answers.
# list the files that the user wants to delete
say("Files to delete:");
foreach (keys %files_hash) 
{  
    if ($files_hash{$_} eq 'y' or $files_hash{$_} eq 'Y') 
    {
        say("$_ will be deleted");
    }
}

print "complete all deletions? [y]: ";
my $input = <STDIN>;  
#DO THE STUFF HERE  
exit 0; 