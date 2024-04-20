#!/usr/bin/perl -w

# Author: Isaiah Harville
# Date: 4/19/2024
# Usage: ./delc.pl test_files/*

use strict;
use warnings;

my @to_delete;

if (@ARGV) {
    my @file_list = glob(join(' ', @ARGV));

    foreach my $file (@file_list) {
        next unless -e $file;

        my $type = -d $file ? "directory" : "file";

        print "Delete $file $type? [y,q] \n";
        chomp(my $input = <STDIN>);

        if ($input eq 'y') {
            push @to_delete, $file;
        }
    }

    if (@to_delete) {
        print "\n-----------\n";
        print join("\n", @to_delete), "\n\n";

        print "Delete all selected files? [y]: ";
        chomp(my $confirm = <STDIN>);
        
        if (lc($confirm) eq 'y') {
            unlink @to_delete;
            print scalar(@to_delete) . " files have been deleted.\n";
        } else {
            print "Nothing has been deleted.\n";
        }
    } else {
        print "No files were selected for deletion.\n";
    }

} else {
    print "ERROR: Missing Args\n";
    print "Usage: ./delc.pl [dir/* | dir | file]";
    exit 1;
}