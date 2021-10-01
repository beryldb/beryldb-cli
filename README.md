
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/actions"><img src="https://github.com/beryldb/beryldb-cli/workflows/Ubuntu%20build/badge.svg?2"></a>
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/actions"><img src="https://github.com/beryldb/beryldb-cli/workflows/macOS%20build/badge.svg?2"></a>
<a target="_blank" href="https://github.com/beryldb/beryldb-cli/pulse" alt="Activity"> <img src="https://img.shields.io/github/commit-activity/m/beryldb/beryldb-cli?2" /></a>
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Mailing List](https://img.shields.io/badge/email-google%20groups-4285F4 "beryldb@googlegroups.com")](https://groups.google.com/g/beryldb)
[![Twitter](https://img.shields.io/twitter/follow/beryldb?color=%23179CF0&logo=twitter&style=flat-square "@beryldb on Twitter")](https://twitter.com/beryldb)
[![Discord Server](https://img.shields.io/discord/823028202318200912?color=7289da&logo=discord "Discord Server")](https://discord.gg/GRCEuMdYRt)
<br>

This README is brief introduction to Beryl-cli. For extended information, you
can visit our documentation site at [docs.beryl.dev/clients/cli](https://docs.beryl.dev/cli/installation)

![Logo](https://docs.beryl.dev/img/smaller.png??)

## What is Beryl-cli?

Beryldb-cli (also known as Emerald) is a lightweight client for [BerylDB](https://github.com/beryldb/beryldb). 

Beryl-cli is a command line interface, it is a program that allows
users to send commands to Beryl and read responses.

If you are planning to use Beryl extensively, or if you already do, 
chances are you familiar with beryl-cli.

Don't forget to follow us on [Twitter](https://twitter.com/beryldb)!

Beryl-cli is supported in MacOS, BSD and Linux systems.

## Installation

Clone latest stable version:

```
git clone --branch 1.0 https://github.com/beryldb/beryldb-cli.git --depth=5
cd beryldb-cli/
```

You may now build Beryl-cli:

```
./configure
make -j2 install
```

## Running

```
./beryl-cli
```

In case you are running your server in a remote server:

```
./beryl-cli --host=example.beryl.dev
```

## Usage

```
Usage: ./beryl-cli <arguments>

     --login <username>,          Login with a different username.
     --host <host>,               Connect to provided host.
     --password <password>,       Use a different password than default.
     --port <port>,               Use a differnt port than 6378
     --use <use>,                 Use a different select than 1.
     --join <channels>,           Join channels upon connection.
     --test,                      Run tests and exit.
     --version,                   Display version and exit.
```

<br>

If you wish to see examples about these parameters, visit our Running [guide](https://docs.beryl.dev/cli/running/).

## Discussions

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

* [Documentation](https://docs.beryl.dev/cli/installation)
* [GitHub](https://github.com/beryldb/beryldb-cli)
* [Support/Discord](https://discord.gg/sqsXVYuGrX)
* [Twitter](https://twitter.com/beryldb)

