# ATestCov User's Guide

## Overview

ATestCov is the software testing tool to mesure n-wise coverage.
ATestCov supports for pairwise testing, orthogonal array testing, and n-wise testing.

## Download

https://github.com/hiro-iseri/atestcov/releases

## Usage

ATestCov is a command-line tool.

```
atestcov [options]
```

[options]
```
  --testcase, -t  : testcase file
  --param, -p     : parameter file
```

### Example

Command Example:

```
atestcov --testcase testcase.txt --param parameter.txt
```

Result Example:

```
*** atestcov(ver.0.02) ***

[coverage report]
number of test case: 1
number of parameter: 3
1wise coverage: 37.50%(3/8)
2wise coverage: 14.29%(3/21)
3wise coverage: 5.56%(1/18)
```

## Detailed Usage 

### Specify N of N-Wise Testing

ATestCov receives N values in the range between the upper and lower limits. ATestCov calculates coverage within range.

N-Value Options:

[options]
```
--lower, -l  :lower value of N (default:1)
--upper, -u  :upper value of N (default:number of parametor in specifiged parametor file.)
```

e.g. if "-l 1 -u 3" is specfied, ATestCov mesures 1, 2, and 3 wise coverage.

### Input File Format

#### Common Format Rule

- Lines prefixed with "#" character and empty lines are ignored.
- Empty keyword is not allowed.
- A spaces before and after the keyword are trimmed.
- keyword cannot contain ":",  "," , and "@mutex".
- The character code follows the  standard character code of the execution environment.

#### Testcase File Format Rule

```
<Parameter1 name>, <Parameter2 name>, ...
<Parameter1 value of testcase1>, <Parameter2 value of testcase1>, 
...
<Parameter1 value of testcase2>, <Parameter2 value of testcase2>, 
...
```

- keywords separated by commas or tabs.
- The first valid line is testcase parameter name.
The second and subsequent lines are parameter value. 
- Format is compatible with PICT output.


Example:

```
OS, Browser
Win7, Edge
Win8, Chrome
Win10, Firefox
```

#### Parameter file format rule

Parameter file describes parameter name and value in test case.

```
<Parameter1 name> : <Value1>, <Value2>, ...
<Parameter2 name> : <Value1>, <Value2>, ...
...
```

- Parameter name and value are separated by ":".
values are separated by ",".
- Format is compatible with PICT input file.

Example:

```
OS: Win7, Win8, Win10
Browswer: Edge, Chrome, Firefox
```

@mutex allow you to specify limitations on a parameter combinations.
Combinations specified @mutex are ignored in coverage measurement

```
@mutex <Parameter name>:<Parameter Value>, <Parameter name>:<Parameter Value>, ...
```

Example:

```
OS: Win7, Win8, Win10
Browswer: Edge, Chrome, firefox

@mutex OS:Win7, Browser:Edge
```


#### Output Format

Coverage report is printed to the standard output stream. 
N-wise coverage is printed after  "[coverage report]". 

Format:

```
<N Value>wise coverage: <N-wise Coverage>%(<number of combinations that appeared in the test case>/<number of all combinations>)
```

Example:

```
[coverage report]
number of test case: 1
number of parameter: 3
1wise coverage: 37.50%(3/8)
2wise coverage: 14.29%(3/21)
3wise coverage: 5.56%(1/18)
```

### Other Option

[options]
```
--addmetrics, -a  : output combination metrics
--help, -h        : print help
--info, -i        : print detailed log
```
