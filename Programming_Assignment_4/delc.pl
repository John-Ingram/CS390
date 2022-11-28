#!/usr/bin/perl

#John Ingram
#CS390 Programming Assignment 4

use strict;
use warnings;
use diagnostics;
use File::Path qw(remove_tree);

use feature 'say';
use feature "switch";

use v5.16;


my @file_list;
my @delete_list;

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

foreach (@file_list) 
{  
    # ask the user if they want to delete the file, if they do, add it to the delete list.
    # if they answer q, then break out of the loop and go to the final confirmation
    say("Delete $_? (y/q)");
    my $answer = <STDIN>;
    chomp $answer;

    if ($answer eq 'y' || $answer eq 'Y') 
    {
        push @delete_list, $_;
    }
    elsif ($answer eq 'q' || $answer eq 'Q') 
    {
        last;
    }
} 

# if the user dosn't want to delete any files, exit
if (@delete_list == 0) 
{
    say("No files to delete");
    exit 0;
}

# now we have a hash of files and answers.
# list the files that the user wants to delete
say("Files to delete:");
foreach (@delete_list) 
{  
    say("$_ will be deleted");
}

# ask the user if they are sure they want to delete the files
print "complete all deletions? [y]: ";
my $input = <STDIN>;  

# if the user says yes, delete the files
if ($input =~ /^y/i) 
{
    foreach (@delete_list) 
    {
        unlink $_;
        # if the file is a directory, delete it
        if (-d $_) 
        {
            remove_tree($_);
        }
    }
}
else 
{
    say("Deletions aborted");
}

exit 0; 