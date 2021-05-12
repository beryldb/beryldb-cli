# Emerald - A POSIX client for BerylDB.
# http://www.beryldb.com
#
# Copyright (C) 2015-2021 Carlos F. Ferry <cferry@beryldb.com>
# 
# This file is part of BerylDB. BerylDB is free software: you can
# redistribute it and/or modify it under the terms of the BSD License
# version 3.
#
# More information about our licensing can be found at https://docs.beryl.dev

package make::brld;

use v5.10.0;
use strict;
use warnings FATAL => qw(all);

use Class::Struct         qw(struct);
use Exporter              qw(import);
use File::Path            qw(mkpath);
use File::Spec::Functions qw(rel2abs);

our @EXPORT = qw(command
                 execute_command
                 brld_format
                 show_error
                 print_ok
                 show_warning
                 prompt_bool
                 interactive_dir
                 prompt_string);

my %BRLD_COLOR = (
	DEFAULT   => "\e[0m",
	BOLD      => "\e[1m",
	UNDERLINE => "\e[4m",

	RED    => "\e[1;31m",
	GREEN  => "\e[1;32m",
	YELLOW => "\e[1;33m",
	BLUE   => "\e[1;34m"
);

my %commands;

struct 'command' => 
{
	'callback'    => '$',
	'description' => '$',
};

sub brld_format($) 
{
	my $message = shift;

	while ($message =~ /(<\|(\S+)\s(.*?)\|>)/) 
	{
		my ($match, $type, $text) = ($1, uc $2, $3);
	
		if (-t STDOUT && exists $BRLD_COLOR{$type}) 
		{
			$message =~ s/\Q$match\E/$BRLD_COLOR{$type}$text$BRLD_COLOR{DEFAULT}/;
		} 
		else 
		{
			$message =~ s/\Q$match\E/$text/;
		}
	}

	return $message;
}

sub show_error 
{
	
	print STDERR brld_format "[ <|RED ERROR|> ] ";

	for my $line (@_) 
	{
		say STDERR brld_format $line;
	}
	
	exit 1;
}

sub show_warning 
{
	print STDERR brld_format "[ <|RED INFO|> ] ";
	
	for my $line (@_) 
	{
		say STDERR brld_format $line;
	}
}

sub print_ok
{
        print STDERR brld_format "[ <|GREEN OK|> ] ";

        for my $line (@_) 
        {
                say STDERR brld_format $line;
        }
}

sub prompt_bool($$$) 
{
	my ($interactive_setup, $question, $default) = @_;

	while (1) 
	{
		my $answer = prompt_string($interactive_setup, $question, $default ? 'yes' : 'no');
		return 1 if $answer =~ /^y(?:es)?$/i;
		return 0 if $answer =~ /^no?$/i;
		show_warning "\"$answer\" is not \"yes\" or \"no\". Please try again.\n";
	}
}

sub interactive_dir($$$;$) 
{
	my ($interactive_setup, $question, $default, $create_now) = @_;
	my ($answer, $create);

	do 
	{
		$answer = rel2abs(prompt_string($interactive_setup, $question, $default));
		$create = prompt_bool($interactive_setup && !-d $answer, "$answer does not exist. Create it?", 'y');

		if ($create && $create_now) 
		{
			unless (create_directory $answer, 0750) 
			{
				show_warning "unable to create $answer: $!\n";
				$create = 0;
			}
		}
	} while (!$create);
	
	return $answer;
}

sub prompt_string($$$) 
{
	my ($interactive_setup, $question, $default) = @_;
	return $default unless $interactive_setup;
	say brld_format $question;
	print brld_format "[<|GREEN $default|>] => ";
	chomp(my $answer = <STDIN>);
	return $answer ? $answer : $default;
}

sub command($$$) 
{
	my ($name, $description, $callback) = @_;
	$commands{$name} = command->new;
	$commands{$name}->callback($callback);
	$commands{$name}->description($description);
}

sub command_alias($$) 
{
	my ($source, $target) = @_;
	
	command $source, undef, sub(@) 
	{
		execute_command $target, @_;
	};
}

sub execute_command(@) 
{
	my $command = defined $_[0] ? lc shift : 'help';

	if ($command eq 'help') 
	{
		say brld_format "<|GREEN Usage:|> $0 <<|UNDERLINE COMMAND|>> [<|UNDERLINE OPTIONS...|>]";
		say '';
		say brld_format "<|GREEN Commands:|>";
		
		for my $key (sort keys %commands) 
		{
			next unless defined $commands{$key}->description;
			my $name = sprintf "%-15s", $key;
			my $description = $commands{$key}->description;
			say brld_format "  <|BOLD $name|> # $description";
		}
		
		exit 0;
	} 
	elsif (!$commands{$command}) 
	{
		show_error "Unable to find module: <|BOLD $command|>",
			"Try <|BOLD $0 help|> to check a list of full commands.";
	} 
	else 
	{
		return $commands{$command}->callback->(@_);
	}
}

1;
