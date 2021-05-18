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

package make::utils;

use v5.10.0;
use strict;
use warnings FATAL => qw(all);

use Exporter              qw(import);
use File::Path            qw(mkpath);
use File::Spec::Functions qw(rel2abs);

use make::brld;

our @EXPORT = qw(create_directory
                 execute
                 get_cpu_count
                 get_version
                 get_distro_version
                 read_config_file
                 write_config_file);

sub create_directory($$) 
{
	my ($location, $permissions) = @_;

	return eval 
	{
		mkpath($location, 0, $permissions);
		return 1;
	} // 0;
}

sub execute(@) 
{
	say brld_format "<|BOLD \$|> @_";
	return system @_;
}

sub get_version 
{
	state %version;
	return %version if %version;

	chomp(my $vf = `sh src/version.sh 2>/dev/null`);

	if ($vf =~ /^Emerald-([0-9]+)\.([0-9]+)\.([0-9]+)(?:-(\w+))?$/) 
	{
		%version = ( MAJOR => $1, MINOR => $2, PATCH => $3, LABEL => $4 );
	}

	chomp(my $gr = `git describe --tags 2>/dev/null`);
	
	if ($gr =~ /^v([0-9]+)\.([0-9]+)\.([0-9]+)(?:[a-z]+\d+)?(?:-\d+-g(\w+))?$/) 
	{
		$version{MAJOR} //= $1;
		$version{MINOR} //= $2;
		$version{PATCH} //= $3;
		$version{LABEL} = $4 if defined $4;
	}

	$version{REAL_LABEL} = $version{LABEL};
	$version{LABEL} = shift // $version{LABEL};

	$version{MAJOR} //= '0';
	$version{MINOR} //= '0';
	$version{PATCH} //= '0';

	if (defined $version{LABEL}) 
	{
		$version{FULL} = "$version{MAJOR}.$version{MINOR}.$version{PATCH}-$version{LABEL}"
	} 
	else 
	{
		$version{LABEL} = 'release';
		$version{FULL} = "$version{MAJOR}.$version{MINOR}.$version{PATCH}"
	}

	return %version;
}

sub get_distro_version
{
        my $version;
        chomp($version = `lsb_release --id --short`);
        return $version;
}

sub get_cpu_count 
{
	my $count = 1;
	
	if ($^O eq 'linux') 
	{
		$count = `getconf _NPROCESSORS_ONLN 2>/dev/null` || 1;
	} 
	
	chomp($count);
	return $count;
}

sub read_config_file($) 
{
	my $path = shift;
	my %config;
	open(my $file_handler, $path) or return %config;

	while (my $line = <$file_handler>) 
	{
		next if $line =~ /^\s*($|\#)/;
		my ($key, $value) = ($line =~ /^(\S+)(?:\s(.*))?$/);
		$config{$key} = $value;
	}
	
	close $file_handler;
	return %config;
}

sub write_config_file($%) 
{
	my $path = shift;
	my %config = @_;
	open(my $file_handler, '>', $path) or show_error "unable to write to $path: $!";

	while (my ($key, $value) = each %config) 
	{
		$value //= '';
		say $file_handler "$key $value";
	}
	
	close $file_handler;
}

1;
