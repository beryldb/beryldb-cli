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

package make::configure;

use v5.10.0;
use strict;
use warnings FATAL => qw(all);

use Exporter              qw(import);
use File::Basename        qw(basename dirname);
use File::Spec::Functions qw(abs2rel catdir catfile);

use make::utils;
use make::brld;

use constant CONFIGURE_ROOT          => dirname dirname __FILE__;
use constant CONFIGURE_DIRECTORY     => catdir(CONFIGURE_ROOT, '.configure');
use constant CONF_CACHEFILE    => catfile(CONFIGURE_DIRECTORY, 'brld_cache.cfg');
use constant CONFIGURE_CACHE_VERSION => '1';
use constant CONFIGURE_ERROR_PIPE    => $ENV{VERBOSE} ? '' : '1>/dev/null 2>/dev/null';

our @EXPORT = qw(CONF_CACHEFILE
                 CONFIGURE_CACHE_VERSION
                 CONFIGURE_DIRECTORY
                 cmd_clean
                 cmd_help
                 cmd_update
                 run_test
                 verify_file
                 strict_test
                 test_header
                 module_shrink
                 define_conf_cache
                 retrieve_compiler
                 locate_compiler
                 process_make);


sub __get_models_settings($$$) 
{

	my ($config, $compiler, $version) = @_;

	my %settings = %$config;

	while (my ($key, $value) = each %{$compiler}) 
	{
		$settings{'COMPILER_' . $key} = $value;
	}

	while (my ($key, $value) = each %{$version}) 
	{
		$settings{'VERSION_' . $key} = $value;
	}

	$settings{CONFIGURE_DIRECTORY} = CONFIGURE_DIRECTORY;
	$settings{CONF_CACHEFILE} = CONF_CACHEFILE;
	$settings{SYSTEM_NAME} = lc $^O;

	return %settings;
}

sub __test_compiler($) 
{
	my $compiler = shift;
	return 0 unless run_test("`$compiler`", !system "$compiler -v ${\CONFIGURE_ERROR_PIPE}");
	return 0 unless run_test("`$compiler`", verify_file($compiler, 'compiler.cpp', '-fno-rtti -std=c++14'), 'compatible');
	return 1;
}

sub cmd_clean 
{
	unlink CONF_CACHEFILE;
}

sub cmd_help 
{
	print brld_format <<EOH;
<|GREEN Usage:|> <|BOLD $0 [OPTIONS|>]

<|GREEN PATH OPTIONS|>

  <|BOLD --portable|>           Sets up the installation paths for a portable installation.

  <|BOLD --system|>             Sets up the installation paths for system-wide installation.

  <|BOLD --prefix <DIR>|>       Sets a root install directory..
                       [${\CONFIGURE_ROOT}/run]
                                                                
  <|BOLD --config-dir <DIR>|>   Location where the configuration files and SSL certificates are stored.
                       [${\CONFIGURE_ROOT}/run/conf]
                                
  <|BOLD --data-dir <DIR>|>     Location where the data files are stored.
                       [${\CONFIGURE_ROOT}/run/data]
                                
  <|BOLD --db-dir <DIR>|>       Location where the database files are stored.
                       [${\CONFIGURE_ROOT}/run/db]
                                
  <|BOLD --example-dir <DIR>|>  Location where example configuration files are stored.
                       [${\CONFIGURE_ROOT}/run/conf/examples]
  <|BOLD --log-dir <DIR>|>      
		       Location where log files are stored.
                       [${\CONFIGURE_ROOT}/run/logs]
                                
  <|BOLD --manual-dir <DIR>|>   Location where the manual files are stored.
                       [${\CONFIGURE_ROOT}/run/manuals]
                                
  <|BOLD --module-dir <DIR>|>   Location where the loadable modules are stored.
                       [${\CONFIGURE_ROOT}/run/modules]
                                
  <|BOLD --runtime-dir <DIR>|>  Location where runtime files are stored.
                       [${\CONFIGURE_ROOT}/run/data]
                                
  <|BOLD --script-dir <DIR>|>   Location where the scripts, are stored.
                       [${\CONFIGURE_ROOT}/run]

<|GREEN MISC OPTIONS|>

  <|BOLD --clean|>                       Removes configuration cache file and start
                                	 the interactive configuration format.

                                	 for which the dependencies are available.
  <|BOLD --disable-interactive|>         Disables the interactive configuration wizard.

  <|BOLD --distribution-label <TEXT>|>   Sets a distribution specific version label in
                                the build configuration.

  <|BOLD --gid <ID|NAME>|>               Sets the group to run BerylDB as.

  <|BOLD --help|>                        Displays help message and exits.

  <|BOLD --uid [ID|NAME]|>               Sets the user to run BerylDB as.

  <|BOLD --update|>                      Updates build environment with the settings from cache.

  <|GREEN FLAGS|>

  <|BOLD CXX=<NAME>|>             	 Defines a C++ compiler to use to build beryldb.

  <|BOLD VERBOSE=<0|1>|>         Shows additional information for debugging.

EOH
	exit 0;
}

sub cmd_update 
{
	show_error "You have not ran $0 before. You must run this command before trying to update the generated files." unless -f CONF_CACHEFILE;
	say 'Updating ...';
	my %config = read_config_file(CONF_CACHEFILE);
	$config{EXAMPLE_DIR} //= catdir $config{CONFIG_DIR}, 'examples';
	$config{RUNTIME_DIR} //= $config{DATA_DIR};
	my %compiler = retrieve_compiler($config{CXX});
	my %version = get_version $config{DISTRIBUTION};
	process_make(\%config, \%compiler, \%version);
	say 'Update complete!';
	exit 0;
}

sub run_test($$;$) 
{
	my ($what, $result, $adjective) = @_;
	$adjective //= 'available';
	
	if ($adjective eq 'available')
	{
		print brld_format "Locating <|GREEN $what|> ... ";
	}
	else
	{
                print brld_format "Checking if <|GREEN $what|> is $adjective ... ";
	}
	
	say brld_format($result ? "<|GREEN ok|>" : "<|RED no|>");
	return $result;
}

sub strict_test($$;$) 
{
        my ($what, $result, $adjective) = @_;
        $adjective //= 'available';
        
        if ($adjective eq 'available')
        {
                print brld_format "Locating <|GREEN $what|> ... ";
        }
        else
        {
                print brld_format "Checking if <|GREEN $what|> is $adjective ... ";
        }
        
        say brld_format($result ? "<|GREEN ok|>" : "<|RED no|>");
        
        if ($result eq "")
        {
        	say "\n";
        	show_error "You need $what in order to install BerylDB.\n";
        	exit;
        }
        
        return $result;
}

sub verify_file($$;$) 
{
	my ($compiler, $file, $args) = @_;
	my $status = 0;
	$args //= '';
	$status ||= system "$compiler -o ${\CONFIGURE_ROOT}/__test_$file ${\CONFIGURE_ROOT}/make/test/$file $args ${\CONFIGURE_ERROR_PIPE}";
	$status ||= system "${\CONFIGURE_ROOT}/__test_$file ${\CONFIGURE_ERROR_PIPE}";
	unlink "${\CONFIGURE_ROOT}/__test_$file";
	return !$status;
}

sub test_header($$;$) 
{
	my ($compiler, $header, $args) = @_;
	$args //= '';
	open(my $file_handler, "| $compiler -E - $args ${\CONFIGURE_ERROR_PIPE}") or return 0;
	print $file_handler "#include <$header>";
	close $file_handler;
	return !$?;
}

sub module_shrink($) 
{
	my $module = basename shift;
	$module =~ s/(?:^m_|\.cpp$)//g;
	return $module;
}

sub define_conf_cache(%) 
{
	unless (-e CONFIGURE_DIRECTORY) 
	{
		say brld_format "Creating <|GREEN ${\abs2rel CONFIGURE_DIRECTORY, CONFIGURE_ROOT}|> ...";
		create_directory CONFIGURE_DIRECTORY, 0750 or show_error "unable to create ${\CONFIGURE_DIRECTORY}: $!";
	}

	say brld_format "Writing <|GREEN ${\abs2rel CONF_CACHEFILE, CONFIGURE_ROOT}|> ...";
	my %config = @_;
	write_config_file CONF_CACHEFILE, %config;
}

sub retrieve_compiler($) 
{
	my $binary = shift;
	my %info = (NAME => 'Unknown', VERSION => '0.0');
	return %info if system "$binary -o __compiler_info ${\CONFIGURE_ROOT}/make/test/compiler_info.cpp ${\CONFIGURE_ERROR_PIPE}";
	open(my $file_handler, '-|', './__compiler_info 2>/dev/null');

	while (my $line = <$file_handler>) 
	{
		$info{$1} = $2 if $line =~ /^([A-Z]+)\s(.+)$/;
	}
	
	close $file_handler;
	unlink './__compiler_info';
	return %info;
}

sub locate_compiler 
{
	my @compilers = qw(g++ c++);
	
	for my $compiler (shift // @compilers) 
	{
		return $compiler if __test_compiler $compiler;
	}
}

sub process_make($$$) 
{

	my ($config, $compiler, $version) = @_;

	my %settings = __get_models_settings($config, $compiler, $version);

	for my $models (<${\CONFIGURE_ROOT}/make/models/*>) 
	{
		say brld_format "Parsing <|GREEN ${\abs2rel $models, CONFIGURE_ROOT}|> ...";
		open(my $file_handler, $models) or show_error "unable to read $models: $!";
		my (@lines, $mode, @platforms, @targets);

		while (my $line = <$file_handler>) 
		{
			chomp $line;

			while ($line =~ /(@(\w+?)(?:\|(\w*))?@)/) 
			{
				my ($variable, $name, $default) = ($1, $2, $3);
				
				if (defined $settings{$name}) 
				{
					$line =~ s/\Q$variable\E/$settings{$name}/;
				} 
				elsif (defined $default) 
				{
					$line =~ s/\Q$variable\E/$default/;
				} 
				else 
				{
					show_warning "Unknown models variable '$name' in $models!";
					last;
				}
			}

			if ($line =~ /^(\s*)%(\w+)\s+(.+)$/) 
			{
				if ($2 eq 'define') 
				{
					if ($settings{$3}) 
					{
						push @lines, "#$1define $3";
					} 
					else 
					{
						push @lines, "#$1undef $3";
					}
				} 
				elsif ($2 eq 'mode') 
				{
					$mode = oct $3;
				} 
				elsif ($2 eq 'platform') 
				{
					push @platforms, $3;
				} 
				elsif ($2 eq 'target') 
				{
					push @targets, catfile CONFIGURE_ROOT, $3;
				} 
				else 
				{
					show_warning "Unknown models command '$2' in $models!";
					push @lines, $line;
				}
				
				next;
			}
			push @lines, $line;
		}

		close $file_handler;

		if ($#platforms < 0 || grep { $_ eq $^O } @platforms) 
		{

			unless (@targets) 
			{
				push @targets, catfile(CONFIGURE_DIRECTORY, basename $models);
			}

			for my $target (@targets) 
			{

				my $directory = dirname $target;
				
				unless (-e $directory) 
				{
					say brld_format "Creating <|GREEN ${\abs2rel $directory, CONFIGURE_ROOT}|> ...";
					create_directory $directory, 0750 or show_error "Unable to create directory: $directory: $!";
				};

				say brld_format "Writing <|GREEN ${\abs2rel $target, CONFIGURE_ROOT}|> ...";
				open(my $file_handler, '>', $target) or show_error "Unable to write target: $target: $!";

				for (@lines) 
				{
					say $file_handler $_;
				}
				
				close $file_handler;

				if (defined $mode) 
				{
					chmod $mode, $target;
				}
			}
		}
	}
}

1;
