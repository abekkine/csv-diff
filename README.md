# CsvDiff - A command line csv comparison tool

CsvDiff is a command line tool that compares two csv files and prints statistics about the differences.

## Statistics

Following statistics are calculated and printed:

* Min / max differences between values
* Mean of differences
* Standard deviation of differences
* Mean of absolute differences
* Standard deviation of absolute differences

> [!NOTE]
> For min / max values, the absolute difference is used.

### Used formulas

In the formulas below,
* $x_i = ref_i - data_i $
used for normal calculations,
* $x_i = |ref_i - data_i| $
used for absolute calculations.

Mean Value : $\bar x = \frac{1}{n} {\sum\limits_{i = 1}^n {{x_i}} } $

Standard Deviation Value :  ${\sigma} = \sqrt {\frac{1}{n}{\sum\limits_{i = 1}^n {{{\left( {{x_i} - \bar x} \right)}^2}} }} $

## Features

* `F01` Header column order is not important; columns are matched by name.
* `F02` For not exactly matching column names, the closest match is suggested, and can be used optionally.
* `F03` Differences calculated according to some `epsilon` value (default is `1e-8`), which can be set by the user.
* `F04` Difference values which are less than `epsilon` are considered as zero.
* `F05` Columns with all differences are zero reported as `Same`.
* `F09` Same lines can be hidden optionally, with only number of such lines reported.
* `F06` If a column has a `NaN` value, then all statistics related to that column are also reported as `NaN`.
* `F10` NaN lines can be hidden optionally, with only number of such lines reported.
* `F07` Report is printed using the csv headers from the ref file, or the data file, optionally.
* `F08` `CHECK` Supports csv files with different number of columns.

### New Features

* `F11` Show total column count int the report.
* `F12` Show matching & total line counts in the report.
* `F13` Report line count mismatch briefly, instead of for each column.
* `F14` Add a brief report mode, for quick checks.
* `F15` Support comparison of columns containing nan values. Conflicts `F06`.

## Installation

TBD

## Usage

```
> csvDiff <reference_file> <data_file> [options]
```

### Options

`--eps=<value>`
Sets the epsilon value. Default is `1e-8`.

`--match`
If a name matching suggestion is available, it is used.

`--hide-same`
Report only the number of columns which have same values (according to epsilon).

`--hide-nan`
Report only the number of columns which have `NaN` values.

`--use-data-names`
If they do not match, use CSV headers from data file, instead of reference file.

# References

Version script is based on Andrew Hardin's cmake script released under the MIT license.

https://github.com/andrew-hardin/cmake-git-version-tracking

