#!/usr/bin/env perl
# Emerald - A POSIX client for BerylDB.
# http://www.beryldb.com
#
# Copyright (C) 2021 Carlos F. Ferry <cferry@beryldb.com>
# 
# This file is part of BerylDB. BerylDB is free software: you can
# redistribute it and/or modify it under the terms of the BSD License
# version 3.
#
# More information about our licensing can be found at https://docs.beryl.dev

use strict;
use warnings FATAL => qw(all);

use File::Basename qw(basename dirname);
use FindBin        qw($RealDir);

use lib dirname $RealDir;
use make::utils;

use constant 
{
	BUILDPATH  => $ENV{BUILDPATH},
	SOURCEPATH => $ENV{SOURCEPATH}
};

sub find_output;
sub gendep($);
sub dep_cpp($$$);
sub dep_so($);
sub dep_dir($$);
sub run();

my %f2dep;

run;
exit 0;

sub run() 
{
	create_directory(BUILDPATH, 0770) or die "Unable to create directory: $!";
	chdir BUILDPATH or die "Unable to open directory: $!";
	unlink 'include';
	symlink "${\SOURCEPATH}/include", 'include';
	mkdir $_ for qw/bin obj/;

	open MAKE, '>real.mk' or die "Unable to write pre parser: $!";
	chdir "${\SOURCEPATH}/src";
	run_dynamic();
	close MAKE;
}

sub run_dynamic() 
{
        print MAKE <<END;
VPATH = \$(SOURCEPATH)/src

bad-target:
	\@echo "This Makefile must be run by a sub-make from the source"
	\@echo "in order to set the correct environment variables"
	\@exit 1

all: beryl-cli

END
        my(@core_deps);
        
        for my $file (<*.cpp>) 
        {
                my $out = find_output $file;
                dep_cpp $file, $out, 'gen-o';
                
                push @core_deps, $out;
        }

        my $core_mk = join ' ', @core_deps;
        print MAKE <<END;

bin/beryl-cli: $core_mk
	@\$(SOURCEPATH)/make/links.pl core-ld \$\@ \$^ \$>

beryl-cli: bin/beryl-cli

.PHONY: all bad-target beryl-cli

END

}

sub find_output 
{
        my $file = shift;
        my($path,$base) = $file =~ m#^((?:.*/)?)([^/]+)\.cpp# or die "Bad file $file";

        if ($path eq '') 
        {
                return "obj/$base.o";
        } 
        else 
        {
                die "Can't determine output for $file";
        }
}

sub gendep($) 
{
	my $f = shift;
	my $basedir = $f =~ m#(.*)/# ? $1 : '.';
	return $f2dep{$f} if exists $f2dep{$f};
	$f2dep{$f} = '';
	my %dep;
	my $link = readlink $f;
	
	if (defined $link) 
	{
		$link = "$basedir/$link" unless $link =~ m#^/#;
		$dep{$link}++;
	}
	
	open my $in, '<', $f or die "Could not read $f";

	while (<$in>) 
	{
		if (/^\s*#\s*include\s*"([^"]+)"/) 
		{
			my $inc = $1;
			next if $inc eq 'constants.h' && $inc eq 'config.h' && $f eq '../include/beryl.h';
			my $found = 0;

			for my $loc ("$basedir/$inc", "../include/$inc") 
			{
				next unless -e $loc;
				$found++;
				$dep{$_}++ for split / /, gendep $loc;
				$loc =~ s#^\.\./##;
				$dep{$loc}++;

			}
			if ($found == 0) 
			{
				print STDERR "Unable to find header: $inc for $f\n";
			} 
			elsif ($found > 1 && $basedir ne '../include') 
			{
				print STDERR "Undefined include $inc in $f\n";
			}
		}
	}

	close $in;
	$f2dep{$f} = join ' ', sort keys %dep;
	$f2dep{$f};
}

sub dep_cpp($$$) 
{
	my($file, $out, $type) = @_;
	gendep $file;

	print MAKE "$out: $file $f2dep{$file}\n";
	print MAKE "\t@\$(SOURCEPATH)/make/links.pl $type \$\@ \$(SOURCEPATH)/src/$file \$>\n";
}

sub dep_so($) 
{
	my($file) = @_;
	my $out = find_output $file;

	my $name = basename $out, '.so';
	print MAKE ".PHONY: $name\n";
	print MAKE "$name: $out\n";

	dep_cpp $file, $out, 'gen-so';
	return $out;
}

sub dep_dir($$) 
{
	my($dir, $outdir) = @_;
	my @ofiles;
	opendir DIR, $dir;
	
	for my $file (sort readdir DIR) 
	{
		next unless $file =~ /(.*)\.cpp$/;
		my $ofile = find_output "$dir/$file";
		dep_cpp "$dir/$file", $ofile, 'gen-o';
		push @ofiles, $ofile;
	}
	
	closedir DIR;
	
	if (@ofiles) 
	{
		my $ofiles = join ' ', @ofiles;
		my $name = basename $outdir;
		print MAKE ".PHONY: $name\n";
		print MAKE "$name: $outdir.so\n";
		print MAKE "$outdir.so: $ofiles\n";
		print MAKE "\t@\$(SOURCEPATH)/make/links.pl link-dir \$\@ ${\SOURCEPATH}/src/$dir \$^ \$>\n";
		return 1;
	} 
	else 
	{
		return 0;
	}
}

