#!/usr/bin/perl

use strict;
use warnings;

use feature 'say';

my %actions = (
    0 => "dummy",
    1 => "send_sub",
    2 => "send_rec",
    3 => "send_sub_and_rec",
    4 => "send_command",
    5 => "recv",
    6 => "sstrat",
    7 => "set_heur",
    8 => "set_bm",
    9 => "solve",
    10 => "exit"
);

my %events = (
    -1 => "error",
    0 => "dummy",
    1 => "start",
    2 => "done",
    3 => "sent",
    4 => "nothsent",
    5 => "data_arrived",
    6 => "command_arrived",
    7 => "sstrat",
    8 => "heur_set",
    9 => "bm_set"
);

my $nsteps = 0;
my $nargs = 4;

while (<>) {
    my @a = split;
    my $s = "";

    my $i = 0;

    $s .= "\[$a[$i++]\] ";
    $s .= "$a[$i++]: ";
    $s .= "action ";
    $s .= "$actions{$a[$i++]}";
    $s .= "[";

    my $iold;

    for ($iold = $i; $i < $iold + $nargs; $i++) {
        $s .= "$a[$i]";
        $s .= ($i != $iold + $nargs - 1) ? ',' : ']';
    }

    $s .= " | event ";
    $s .= "$events{$a[$i]}";

    if ($events{$a[$i]} eq "done") {
        $nsteps += $a[++$i];
    }

    $s .= "[";

    for ($iold = $i; $i < $iold + $nargs; $i++) {
        $s .= "$a[$i]";
        $s .= ($i != $iold + $nargs - 1) ? ',' : ']';
    }

    $s .= " |";
    $s .= " nsub = $a[$i++],";
    $s .= " recup = $a[$i]";

    say($s);
    say("\tTotal number of steps is $nsteps");
}
