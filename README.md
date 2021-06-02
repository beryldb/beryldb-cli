<a target="_blank" href="https://twitter.com/beryldb"><img src="https://img.shields.io/twitter/url/https/twitter.com/cloudposse.svg?style=social&label=Follow%20%40beryldb"></a>
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/actions"><img src="https://github.com/beryldb/beryldb-cli/workflows/Ubuntu%20build/badge.svg?2"></a>
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/actions"><img src="https://github.com/beryldb/beryldb-cli/workflows/MacOS%20build/badge.svg?2"></a>
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/pulse" alt="Activity"> <img src="https://img.shields.io/github/commit-activity/m/beryldb/beryldb-cli?2" /></a>
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
<br>

This README is brief introduction to Beryl-cli. For extended information, you
can visit our documentation site at [beryl.dev/clients/cli](https://docs.beryl.dev/clients/cli/installing/)

![Logo](https://docs.beryl.dev/img/smaller.png??)

## What is Beryl-cli?

Beryldb-cli (also known as Emerald) is a lightweight client for Beryl. 

Beryl-cli is a command line interface, it is a program that allows
users to send commands to Beryl and read responses.

If you are planning to use Beryl extensively, or if you already do, 
chances are you familiar with beryl-cli.

Don't forget to follow us on [Twitter](https://twitter.com/beryldb)!

Beryl-cli is supported in MacOS, BSD and Linux systems.

## Installation

Download files:

```
wget https://github.com/beryldb/beryldb-cli/archive/refs/tags/0.2.1.tar.gz
tar -xvzf 0.2.1.tar.gz
cd beryldb-cli-0.2.1/
```

You may now build Beryldb-cli:

```
./configure
make -j2
```

## Running

```
./beryldb-cli
```

## Usage

```
Usage: ./beryl-cli <arguments>

     --login <username>,          Login with a different username.
     --host <host>,               Connect to a different host than localhost.
     --password <password>,       Use a different password than default.
     --port <port>,               Use a differnt port than 6378
     --use <use>,                 Use a different select than 1.
     --join <channels>,           Join channels upon connection.
     --test,                      Run tests and exit.
     --version,                   Display version and exit.

```

<br>

If you wish to see examples about these parameters, visit our Running [guide](https://docs.beryl.dev/clients/cli/running/).

## Internals

In order to maintain a documentation that is easy to follow, we try to maintain 
discussions at a very shallow level. Our code is actively changing and thus
a brief discussion board is always appreacited.

## Source code organization

Beryl-cli's source code is contained in the `src/` directory. The Makefile, which is
created after configuring Beryl, is located in the root directory.

Inside the root directory, you will find:

* `src`: Contains cli's implementation, written in C++ and C.
* `include`: Contains cli headers. 
* `etc`: Libraries developed by 3rd party organizations.
* `make`: Contains several Perl-based functions to compile the cli.

## External Links

* **IRC**: irc.libera.chat - #beryl
* [Documentation](https://docs.beryl.dev/clients/cli/installing/)
* [GitHub](https://github.com/beryldb/beryldb-cli)
* [Support/Discord](https://discord.gg/sqsXVYuGrX)
* [Twitter](https://twitter.com/beryldb)

