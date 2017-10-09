use strict;
use warnings;

use constant BUFFER_SIZE => 400;

my $real_frame_cnt = 0;
my $lagless_frame_cnt = 0;

my $in_file = 'smb_w11_w84_lag.txt'; # replace with your input file
open(my $ifh, '<:encoding(UTF-8)', $in_file)
  or die "Could not open file '$in_file' $!";

my $out_file = 'smb_dh.bin'; # name your output file
open(my $ofh, '>', $out_file)
  or die "Could not open file '$out_file' $!";

my $last_buttons = 0x00;
my $loop_start = 0;
my $loop_end = -1;

# Need to define which frames from input file are lag frames
my @lag_frames = (0..9,14..17,19,20,107,694,1011,2021,2520,3911,6140,6639,7327,7892,10974,13138,15260,16005,16448,16830,17723);

while (my $row = <$ifh>) {
  chomp $row;
  $row = substr($row, 1, 11);
  if (!grep { $_ eq $real_frame_cnt } @lag_frames) {
    my $buttons = 0x00;
    $row = substr($row, 3);
    $buttons |= (substr($row, 0, 1) eq "U") ? 0x10 : 0x00;
    $buttons |= (substr($row, 1, 1) eq "D") ? 0x20 : 0x00;
    $buttons |= (substr($row, 2, 1) eq "L") ? 0x40 : 0x00;
    $buttons |= (substr($row, 3, 1) eq "R") ? 0x80 : 0x00;
    $buttons |= (substr($row, 4, 1) eq "S") ? 0x08 : 0x00;
    $buttons |= (substr($row, 5, 1) eq "s") ? 0x04 : 0x00;
    $buttons |= (substr($row, 6, 1) eq "B") ? 0x02 : 0x00;
    $buttons |= (substr($row, 7, 1) eq "A") ? 0x01 : 0x00;
    syswrite($ofh, pack('C', $buttons), 1);
    $lagless_frame_cnt++;
  }
  $real_frame_cnt++;
}

for (my $idx = ($lagless_frame_cnt % BUFFER_SIZE); $idx < BUFFER_SIZE; $idx++) {
  syswrite($ofh, pack('C', 0x00));
}
