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

package make::environment;

use v5.10.0;
use strict;
use warnings FATAL => qw(all);

use File::Basename        qw(dirname);
use File::Spec::Functions qw(catdir);
use Exporter              qw(import);

use make::configure;
use make::brld;

use constant DIRECTIVE_ERROR_PIPE => $ENV{BERYLDB_VERBOSE} ? '' : '2>/dev/null';
use constant ETC_DIRECTORY     => catdir(dirname(dirname(__FILE__)), 'etc');

our @EXPORT = qw(get_environment
                 run_functions);

sub get_environment($$;$)
{
	my ($file, $property, $default) = @_;
	open(my $file_handler, $file) or return $default;

	my $value = '';

	while (<$file_handler>) 
	{
		if ($_ =~ /^\/\* \$(\S+): (.+) \*\/$/ || $_ =~ /^\/\/\/ \$(\S+): (.+)/) 
		{
			next unless $1 eq $property;
			$value .= ' ' . run_functions($file, $1, $2);
		}
	}
	
	close $file_handler;

	$value =~ s/^\s+|\s+$//g;
	return $value || $default;
}

sub run_functions($$$) 
{
	my ($file, $name, $line) = @_;

	for (our @parameters = (); $line =~ /([a-z_]+)\((?:\s*"([^"]*)(?{push @parameters, $2})"\s*)*\)/; undef @parameters) 
	{
		my $sub = make::environment->can("__function_$1");
		show_error "unknown $name environment '$1' in $file!" unless $sub;

		my $result = $sub->($file, @parameters);

		if (defined $result) 
		{
			$line = $` . $result . $';
			next;
		}

		$line = $`;
	}

	return $line;
}

sub __environment 
{
	my ($prefix, $suffix) = @_;
	$suffix =~ s/[-.]/_/g;
	$suffix =~ s/[^A-Za-z0-9_]//g;
	return $prefix . uc $suffix;
}

sub __error 
{
	my ($file, @message) = @_;
	push @message, '';

	my $dependencies = get_environment($file, 'PackageInfo');

	if (defined $dependencies) 
	{
		my @packages = sort grep { /^\S+$/ } split /\s/, $dependencies;
		push @message, 'You need the following packages to be installed';
		for (@packages) 
		{
			push @message, " * $_";
		}
	} 
	else 
	{
		push @message, 'You need to check that all dependencies are available,';
		push @message, ' so this module can be installed.';
	}

	push @message, '';

	my $author = get_environment($file, 'ModAuthor');

	if (defined $author) 
	{
		push @message, 'You should contact the author of this module if you believe this to be a bug:';
		push @message, 'author of this module:';
		
		my $author_mail = get_environment($file, 'ModAuthorMail');
		
		if (defined $author_mail) 
		{
			push @message, " * $author <$author_mail>";
		} 
		else 
		{
			push @message, " * $author";
		}
	} 
	else 
	{
		push @message, 'If you think this may be a bug, feel free to report it to';
		push @message, 'at https://github.com/beryldev/beryl/issues';
		push @message, '';
	}
	
	push @message, '';

	push @message, 'If you would like help with fixing this problem then visit our GitHub repository.';
	push @message, '';

	show_error @message;
}

sub __function_error 
{
	my ($file, @messages) = @_;
	__error $file, @messages;
}

sub __function_execute 
{
	my ($file, $command, $environment, $defaults) = @_;

	chomp(my $result = `$command ${\DIRECTIVE_ERROR_PIPE}`);
	
	unless ($?) 
	{
		say brld_format "Execution of `<|GREEN $command|>` succeeded: <|BOLD $result|>";
		return $result;
	}

	if (defined $environment && $environment ne '') 
	{
		$environment = __environment 'BERYLDB_', $environment;
	
		if (defined $ENV{$environment}) 
		{
			say brld_format "Execution of `<|GREEN $command|>` failed; using the environment: <|BOLD $ENV{$environment}|>";
			return $ENV{$environment};
		}
	}

	if (defined $defaults) 
	{
		say brld_format "Execution of `<|GREEN $command|>` failed; using the defaults: <|BOLD $defaults|>";
		return $defaults;
	}

	__error $file, "`<|GREEN $command|>` has exited with a exit code different than 0.";
}

sub __function_locate_compiler_flags 
{
	my ($file, $name, $defaults) = @_;

	chomp(my $flags = `pkg-config --cflags $name ${\DIRECTIVE_ERROR_PIPE}`);
	
	unless ($?) 
	{
		say brld_format "Found the <|GREEN $name|> compiler flags for <|GREEN ${\module_shrink $file}|> using pkg-config: <|BOLD $flags|>";
		return $flags;
	}

	my $key = __environment 'BERYLDB_CXXFLAGS_', $name;
	
	if (defined $ENV{$key}) 
	{
		say brld_format "Found the <|GREEN $name|> compiler flags for <|GREEN ${\module_shrink $file}|> using the environment: <|BOLD $ENV{$key}|>";
		return $ENV{$key};
	}

	if (defined $defaults) 
	{
		say brld_format "Using the default <|GREEN $name|> compiler flags for <|GREEN ${\module_shrink $file}|>: <|BOLD $defaults|>";
		return $defaults;
	}

	__error $file, "Unable to locate the required compiler flags for <|GREEN $name|> <|GREEN ${\module_shrink $file}|>!";
}

sub __function_find_linker_flags 
{
	my ($file, $name, $defaults) = @_;

	chomp(my $flags = `pkg-config --libs $name ${\DIRECTIVE_ERROR_PIPE}`);

	unless ($?) 
	{
		say brld_format "Found the <|GREEN $name|> linker flags for <|GREEN ${\module_shrink $file}|> using pkg-config: <|BOLD $flags|>";
		return $flags;
	}

	my $key = __environment 'BERYLDB_CXXFLAGS_', $name;
	
	if (defined $ENV{$key}) 
	{
		say brld_format "Found the <|GREEN $name|> linker flags for <|GREEN ${\module_shrink $file}|> using the environment: <|BOLD $ENV{$key}|>";
		return $ENV{$key};
	}

	if (defined $defaults) 
	{
		say brld_format "Using the default <|GREEN $name|> linker flags for <|GREEN ${\module_shrink $file}|>: <|BOLD $defaults|>";
		return $defaults;
	}

	__error $file, "Unable to locate <|GREEN $name|> linker flags for <|GREEN ${\module_shrink $file}|>!";
}

sub __function_require_compiler 
{
	my ($file, $name, $minimum, $maximum) =  @_;

	return undef unless $ENV{CXX};
	my %compiler = retrieve_compiler($ENV{CXX});

	return undef unless $compiler{NAME} eq $name;
	return undef if defined $minimum && $compiler{VERSION} < $minimum;
	return undef if defined $maximum && $compiler{VERSION} > $maximum;

	return "";
}

sub __function_require_system 
{
	my ($file, $name, $minimum, $maximum) = @_;
	my ($system, $version);

	if ($^O eq 'linux' && $name ne 'linux') 
	{
		chomp($system = lc `lsb_release --id --short 2>/dev/null`);
		chomp($version = lc `lsb_release --release --short 2>/dev/null`);
	}

	chomp($system ||= lc `uname -s 2>/dev/null`);
	chomp($version ||= lc `uname -r 2>/dev/null`);

	$version =~ s/^(\d+\.\d+).+/$1/;

	return undef if $name ne $system;
	return undef if defined $minimum && $version < $minimum;
	return undef if defined $maximum && $version > $maximum;

	return "";
}

sub __function_require_version 
{
	my ($file, $name, $minimum, $maximum) = @_;

	if (system "pkg-config --exists $name ${\DIRECTIVE_ERROR_PIPE}") 
	{
		show_warning "unable to look up the version of <|GREEN $name|> using pkg-config!";
		return undef;
	}

	return undef if defined $minimum && system "pkg-config --atleast-version $minimum $name";
	return undef if defined $maximum && system "pkg-config --max-version $maximum $name";

	return "";
}

sub __function_etc_directory 
{
	my ($file, $name) = @_;

	my $key = __environment 'BERYLDB_ETC_', $name;

	if (defined $ENV{$key}) 
	{
		say brld_format "Found the <|GREEN $name|> etc directory for <|GREEN ${\module_shrink $file}|> using the environment: <|BOLD $ENV{$key}|>";
		return $ENV{$key};
	}

	my $directory = catdir(ETC_DIRECTORY, $name);
	
	if (-d $directory) 
	{
		say brld_format "Using the default <|GREEN $name|> etc directory for <|GREEN ${\module_shrink $file}|>: <|BOLD $directory|>";
		return $directory;
	}

	__error $file, "Unable to find <|GREEN $name|> etc directory for <|GREEN ${\module_shrink $file}|>!";
}

sub __function_warning 
{
	my ($file, @messages) = @_;
	show_warning @messages;
}

1;
