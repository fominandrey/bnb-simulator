#!/usr/bin/perl
# Process logs produced by DM solver

use strict;
use warnings;

use feature 'say';

my @stats;

while (<>) {
  if ($_ =~ /(.*):(.*)/) {
    $stats[$1] = $2;
  }
}

my $sbytes = 0;
my $rbytes = 0;

my $tsteps = 0;
my $ttime = 0;

for (my $i = 0; $i < @stats; $i++) {
  my @a = split(" ", $stats[$i]);

  say("process $i:");
  say("\tsent parcels: $a[0]");
  say("\tsent bytes: $a[1]");
  say("\tsent subproblems: $a[2]");
  say("\tsent records: $a[3]");
  say("\tsent commands: $a[4]");
  say("\treceived parcels: $a[5]");
  say("\treceived bytes: $a[6]");
  say("\treceived subproblems: $a[7]");
  say("\treceived records: $a[8]");
  say("\treceived commands: $a[9]");
  say("\tsteps done: $a[10]");
  say("\ttime spent: $a[11]");
  say("");

  $sbytes += $a[1];
  $rbytes += $a[6];

  $tsteps += $a[10];
  $ttime = $a[11] if ($ttime < $a[11]);
}

my $speed;

if ($ttime > 0) {
    $speed = $tsteps / $ttime;
} else {
    $speed = undef;
}

say("Total steps: $tsteps, total bytes sent: $sbytes, total bytes received: $rbytes, total time: $ttime, processing speed: $speed");
