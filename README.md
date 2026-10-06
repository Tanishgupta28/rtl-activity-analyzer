# rtl-activity-analyzer

An educational C++17 learning project inspired by concepts used in RTL
power-analysis workflows. It does not perform professional RTL power analysis.

RTL means **register-transfer level**, a way to describe digital hardware.
For this project, a digital signal has a value of **0** or **1**. A **trace**
records the values of several signals at successive samples. Later stages will
explore switching activity: how often these values change.

## Current functionality: Stage 1

The program reads a CSV trace, validates it, and prints the number of samples,
the number of signals, and their names. It stores the cycle numbers and all
signal values in memory. It stops with an understandable error if the input
is invalid.

Stage 1 performs parsing only. Activity and power calculations are future work.

## Project structure

```text
rtl-activity-analyzer/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── TraceData.h
│   └── TraceParser.h
├── src/
│   ├── TraceParser.cpp
│   └── main.cpp
└── data/
    └── sample_trace.csv
```

- `CMakeLists.txt` tells CMake which source files to compile and requires C++17.
- `TraceData.h` defines the data stored using standard-library vectors.
- `TraceParser.h` declares the parser's public `parse` function.
- `TraceParser.cpp` reads and validates the CSV file.
- `main.cpp` handles command-line arguments and prints results or errors.
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

Signals:
clk
enable
data_valid
busy
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
  uniqueness are not checked; rows are stored in file order.
- The entire trace is stored in memory.
- There is no transition counting, switching-activity calculation, signal
  ranking, power estimation, VCD parsing, visualization, or multithreading.
- There is no automated test suite yet; Stage 1 is checked by running the
  command manually with valid and invalid files.

## Roadmap: NOT IMPLEMENTED

Each item below is a possible later learning stage, not current functionality:

- Transition counting — **NOT IMPLEMENTED**
- Switching activity — **NOT IMPLEMENTED**
- Signal ranking — **NOT IMPLEMENTED**
- Simple relative power estimation — **NOT IMPLEMENTED**
- Automated tests — **NOT IMPLEMENTED**
- Optional visualization — **NOT IMPLEMENTED**

## Git and GitHub basics

Git stores commits (saved versions of the source) locally in this folder's
`.git/` directory. GitHub holds a remote copy. `origin` is the short name for
that remote, and `git push` uploads local commits. Local commits remain
available even when GitHub is offline.

Useful commands from the root are `git status` to inspect changes,
`git log --oneline` to inspect history, and `git remote -v` to inspect the
GitHub connection. Generated build files are not committed.
