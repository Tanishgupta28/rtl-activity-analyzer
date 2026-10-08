# rtl-activity-analyzer

An educational C++17 learning project inspired by concepts used in RTL
power-analysis workflows. It does not perform professional RTL power analysis.

RTL means **register-transfer level**, a way to describe digital hardware.
For this project, a digital signal has a value of **0** or **1**. A **trace**
records the values of several signals at successive samples. The program now
measures switching activity: how often these sampled values change.

## Current functionality: Stage 3

Stage 1 reads and validates a CSV trace, storing the cycle numbers and signal
values in memory. Its input rules and error handling are preserved.

Stage 2 compares consecutive values for each signal. It prints the sample and
signal counts, then a table of total, rising, and falling transitions and the
activity ratio. This original table keeps signals in CSV header order.

Stage 3 adds a separate high-activity signal ranking after the original table.
It ranks a copy of the results by activity ratio, highest first. Equal activity
values keep their original signal order, and the original results are unchanged.

The activity ratio is an educational, simplified sampled activity metric. It
is not a complete professional RTL power model, and no power is calculated.

## Understanding transitions and activity

A **transition**, also called a **toggle**, happens when a signal changes between
two consecutive samples:

- `0 -> 0` and `1 -> 1`: no transition.
- `0 -> 1`: a **rising transition**.
- `1 -> 0`: a **falling transition**.

The **total transition count** is the number of these changes. For binary
signals, it always equals the rising count plus the falling count.

For example, `0 0 1 1 0 1` has three transitions: two rising and one falling.
Six samples provide five adjacent pairs that could contain transitions. This
project defines:

```text
activity_ratio = transitions / (samples - 1)
```

For this example, `3 / 5 = 0.6`. A constant signal has activity `0.0`; a signal
that changes at every adjacent pair has activity `1.0`. The code uses
floating-point division so fractions are preserved.

With only one sample there are no pairs to compare. All transition counts and
the activity ratio remain zero, and the program skips division to avoid
dividing by zero. This exception is part of the project's definition.

The analyzer starts at sample index `1` and compares each value with the value
at index `sample_index - 1`. It counts each change and its direction, then
computes the ratio. It takes the validated trace by `const` reference, so it
does not copy or modify the input. Each `SignalActivity` result starts with
zero counts and a ratio of `0.0`.

## Understanding high-activity ranking

High switching activity means a signal changes frequently in the sampled trace.
Ranking helps identify signals worth investigating first. This remains an
educational switching-activity analyzer, not a professional power estimator.

`rankByActivity` receives the results by `const` reference, makes a vector copy,
and uses `std::stable_sort` on that copy. Its comparator is
`left.activity_ratio > right.activity_ratio`, which puts higher ratios first.
The comparison uses the stored ratios before display rounding.

Unlike `std::sort`, `std::stable_sort` guarantees that equal activity values
keep their input order. For the sample, `enable` and `data_valid` both have
activity `0.400`, so `enable` stays first. There is no secondary ranking rule.
The method returns the ranked copy, preserving the original CSV order for the
Stage 2 table.

## Project structure

```text
rtl-activity-analyzer/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── TraceData.h
│   ├── TraceParser.h
│   └── ActivityAnalyzer.h
├── src/
│   ├── TraceParser.cpp
│   ├── ActivityAnalyzer.cpp
│   └── main.cpp
└── data/
    └── sample_trace.csv
```

- `CMakeLists.txt` tells CMake which source files to compile and requires C++17.
- `TraceData.h` defines the data stored using standard-library vectors.
- `TraceParser.h` declares the parser's public `parse` function.
- `TraceParser.cpp` reads and validates the CSV file.
- `ActivityAnalyzer.h` defines `SignalActivity` and declares `analyze` and
  `rankByActivity`.
- `ActivityAnalyzer.cpp` counts transitions, computes activity ratios, and
  ranks a copy of the results.
- `main.cpp` handles arguments, runs parsing and analysis, and prints the
  original table and ranking or errors. Ratios use three decimal places.
- `sample_trace.csv` is a small example with six samples and four signals.
- `.gitignore` keeps generated build files and local editor settings out of Git.

## Build on Linux

Install a C++17-capable compiler, CMake 3.16 or newer, and a build tool such as
Make. Open a terminal in the repository's root folder, then run:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

`cmake ..` reads the project configuration from the parent folder and generates
build files. `cmake --build .` uses those files to compile the program. The
generated files stay in `build/`, which Git ignores.

## Example execution

From the `build/` folder:

```bash
./rtl_activity_analyzer ../data/sample_trace.csv
```

`./` means the current folder; `../` means its parent folder. The CSV path is
relative to the folder where you run the program.

Expected output:

```text
Trace loaded successfully
Samples: 6
Signals: 4

Switching Activity
Signal            Transitions  Rising  Falling  Activity
clk               5            3       2        1.000
enable            2            1       1        0.400
data_valid        2            1       1        0.400
busy              1            1       0        0.200

High-Activity Signal Ranking

Rank  Signal            Activity  Transitions
1     clk               1.000     5
2     enable            0.400     2
3     data_valid        0.400     2
4     busy              0.200     1
```

The command accepts exactly one file path. Use quotes around a path containing
spaces. Success returns exit code `0`; incorrect arguments or invalid input
print a message to standard error and return exit code `1`.

On Windows with CMake and MinGW on your PATH, use these commands from the root:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\rtl_activity_analyzer.exe data\sample_trace.csv
```

## Example input and parser rules

```csv
cycle,clk,enable,data_valid,busy
0,0,0,0,0
1,1,0,0,0
2,0,1,0,0
3,1,1,1,0
4,0,1,1,1
5,1,0,0,1
```

- The first row is the header; its first field must be exactly `cycle`.
- At least one signal is required. Signal names must be nonempty and unique.
- Every data row must have the same number of columns as the header.
- A cycle is a non-negative decimal integer that fits in `unsigned long long`.
  Negative numbers, fractions, text, and values outside that range are rejected.
- Each signal value must be exactly `0` or `1`.
- Spaces and tabs around fields are removed. Linux and Windows line endings
  are accepted.
- Empty files, headers without data, blank rows, and empty data fields are
  rejected. Errors identify the line and signal or column where relevant.

`TraceData` stores three related vectors:

- `cycles[sample_index]` contains that sample's cycle number.
- `signal_names[signal_index]` contains that signal's name.
- `signal_values[signal_index][sample_index]` contains its value at that sample.

For example, `signal_names[0]` is `clk`, `cycles[2]` is `2`, and
`signal_values[0][2]` is `0`. C++ vector indices start at zero.

## Current limitations

- Only simple, unquoted, comma-separated CSV is supported. Quoted fields,
  escaped commas, comments, and a UTF-8 byte-order mark are unsupported.
- Only binary values are supported; unknown (`X`) and high-impedance (`Z`)
  states are unsupported.
- Cycle numbers are sample labels, not physical timestamps. Their order and
  uniqueness are not checked; rows are compared in file order. Gaps between
  cycle numbers do not change the denominator: it is still `samples - 1`.
- The entire trace is stored in memory.
- The ratio describes changes between recorded samples; it cannot measure
  changes that happen between those samples.
- There is no power estimation, VCD parsing, visualization, or multithreading.
- There is no automated test suite yet. Verification uses manual runs with
  the sample, constant signals, alternating signals, a single sample, multiple
  signals, equal and zero activity, ranking order, and invalid CSV inputs.

## Roadmap

Implemented:

- CSV trace parsing — **IMPLEMENTED**
- Transition counting — **IMPLEMENTED**
- Rising/falling transition counting — **IMPLEMENTED**
- Simplified switching activity ratio — **IMPLEMENTED**
- High-activity signal ranking — **IMPLEMENTED**

Possible future learning stages:

- Simple relative power estimation — **NOT IMPLEMENTED**
- Automated tests — **NOT IMPLEMENTED**
- VCD parsing — **NOT IMPLEMENTED**
- Optional visualization — **NOT IMPLEMENTED**
- Multithreading — **NOT IMPLEMENTED**

## Git and GitHub basics

Git stores commits (saved versions of the source) locally in this folder's
`.git/` directory. GitHub holds a remote copy. `origin` is the short name for
that remote, and `git push` uploads local commits. Local commits remain
available even when GitHub is offline.

Useful commands from the root are `git status` to inspect changes,
`git log --oneline` to inspect history, and `git remote -v` to inspect the
GitHub connection. Generated build files are not committed.
