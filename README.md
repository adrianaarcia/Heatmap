# Heatmap
Produce heatmap from GPS data.
Last Updated: 10/24/2019.

## Introduction
A heatmap is a graphical representation of a 2-D array using different colors to represent different ranges of values. Heatmaps are useful in a variety of applications, including web usability, sports analytics, and many geographic applications.
This application is search-and-rescue: given a GPS track, we want to see where in the search area the track hasn't been as often so that we can focus future search efforts on those locations.

## `track` ADT
A GPS track is a collection of trackpoints each containing a location and timestamp. Trackpoints within tracks are ordered by increasing timestamp and can be organized into multiple segments within the same track (a tracking device might, for example, start a new segment each time the user pauses the device, starts a new lap, or when the devices loses the signal from the satellites).
The header file `track.h` specifies the functions for the `track` abstract data type, which models as GPS track.

## `Heatmap` program
I use the `track` ADT implementation to write a program that produces heatmaps for search-and-rescue operations. The size of the geographic regions represented by each cell in the heatmap and which characters used to represent which values will be specified as command-line arguments. The trackpoints are read from standard input.
The command-line arguments will be in the following order:

* the width, in degrees of longitude, of each cell in the heatmap, which should be positive and in a format readable by `atof`;
* the height, in degrees of latitude, of each cell in the heatmap, which should be positive and in a format readable by `atof`;
* a non-empty string of characters used to represent ranges of values in the heatmap, with the character used to represent the lowest range of values first and the character used to represent the highest range last; and
* a positive integer _n_ in a format readable by `atoi` that defines the ranges of values represented by each character, so that if the length of the string of characters is _m_ then the ranges are _kn,…,(k+1)n−1_ for _k = 0,…,m−2,_ and _(m−1)n,…,∞_.

Standard input should contain one trackpoint per line, with a single blank line before the second and subsequent track segments. The latitude and longitude for each trackpoint should be given in degrees in a format readable by `atof` and the values will satisfy the preconditions of `trackpoint_create`. 
The time should be given as the number of milliseconds since January 1, 1970 in a format readable by `atol`. All three fields should be separated by single space characters, with no leading or trailing whitespace (aside from the newline that marks the end of the line). The trackpoints should be given in order of increasing timestamp. There should be a single empty line used to delimit segments within the track, and if there is more than one segment they should all be non-empty.

The output is a heatmap representing all trackpoints and all segments read from standard input, as specified by the `heatmap` function in the `track` ADT, using the cell width and height given on the command line. 
Each integer value in the heatmap is replaced with a single character as determined by the last two command-line arguments, with no space between characters in the same row. 

## Example
```
$ for N in `seq 1 13`; do ./Unit $N; done
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
PASSED
```
```
$ ./Heatmap 1.0 1.0 .xX* 1 < Tests/heatmap_2x2.in
*.
xX
```
```
$ ./Heatmap 0.03125 0.03125 .xX* 1 < Tests/grid_walk_6_4.in
xxxxX
xxxxX
XXXX*
```
```
$ ./Heatmap 0.003 0.002 .:xX# 10 < Tests/east_rock.in
.........X#......
.........#X......
.........#x......
.........#.......
....Xxx####......
....x..#.###.....
....x.....###....
....#......#x....
:XX#X......x.....
x...........#.xX.
x...........XXx:x
X...............x
.X..............x
x#.............X.
#.......#......x.
#.......Xx#....X.
.X#x:...X.:...X..
...##XxX#..#X#x..
```
