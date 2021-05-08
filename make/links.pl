#!/usr/bin/env perl

#
# BerylDB - A modular database.
# http://www.beryldb.com
#
# Copyright (C) 2015-2021 Carlos F. Ferry <cferry@beryldb.com>
# 
# This file is part of BerylDB. BerylDB is free software: you can
# redistribute it and/or modify it under the terms of the BSD License
# version 3.
#
# More information about our licensing can be found at https://docs.beryl.dev

use v5.10.0;
use strict;
use warnings FATAL => qw(all);

use File::Basename        qw(dirname);
use File::Spec::Functions qw(abs2rel);
use FindBin               qw($RealDir);

use lib dirname $RealDir;

use make::brld;
use make::environment;

chdir $ENV{BUILDPATH};

my $type = shift;
my $out = shift;

if ($type eq 'core-ld') 
{
	do_core_link(@ARGV);
} 
elsif ($type eq 'link-dir') 
{
	do_link_dir(@ARGV);
} 
elsif ($type eq 'gen-o') 
{
	do_compile(1, 0, @ARGV);
} 
elsif ($type eq 'gen-so') 
{
	do_compile(1, 1, @ARGV);
} 
elsif ($type eq 'link-so') 
{
	do_compile(0, 1, @ARGV);
} 
else 
{
	print STDERR "Unknown links subcommand $type!\n";
}
exit 1;

sub message($$$) 
{
	my ($type, $file, $command) = @_;

	if ($ENV{BERYLDB_VERBOSE}) 
	{
		say $command;
	} 
	else 
	{
		say brld_format "\t<|GREEN $type:|>\t\t$file";
	}
}

sub rpath($) 
{
	my $message = shift;
	$message =~ s/-L(\S+)/-Wl,-rpath,$1 -L$1/g unless defined $ENV{BERYLDB_DISABLE_RPATH};
	return $message;
}

sub do_core_link 
{
	my $execstr = "$ENV{CXX} -o $out $ENV{CORELDFLAGS} @_ $ENV{LDLIBS}";
	message 'LINK', $out, $execstr;
	exec $execstr;
}

sub do_link_dir {
	my ($dir, $link_flags) = (shift, '');
	for my $file (<$dir/*.cpp>) {
		$link_flags .= rpath(get_environment($file, 'LinkerFlags', '')) . ' ';
	}
	my $execstr = "$ENV{CXX} -o $out $ENV{PICLDFLAGS} @_ $link_flags";
	message 'LINK', $out, $execstr;
	exec $execstr;
}

sub do_compile 
{
	my ($do_compile, $do_link, $file) = @_;

	my $flags = '';
	my $libs = '';

	if ($do_compile) 
	{
		$flags = $ENV{CORECXXFLAGS} . ' ' . get_environment($file, 'CompilerFlags', '');

		if ($file =~ m#(?:^|/)((?:m|core)_[^/. ]+)(?:\.cpp|/.*\.cpp)$#) 
		{
			$flags .= ' -DMODNAME=\\"'.$1.'\\"';
		}
	}

	if ($do_link) 
	{
		$flags = join ' ', $flags, $ENV{PICLDFLAGS};
		$libs = rpath(get_environment($file, 'LinkerFlags', ''));
	} 
	else 
	{
		$flags .= ' -c';
	}

	my $execstr = "$ENV{CXX} -o $out $flags $file $libs";
	message 'BUILD', abs2rel($file, "$ENV{SOURCEPATH}/src"), $execstr;
	exec $execstr;
}
