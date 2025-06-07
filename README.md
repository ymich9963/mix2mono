# Mix2Mono - Multi-channel to Single-channel Audio Files
![badge](https://badgen.net/badge/Coverage/100/blue) ![coverity](https://scan.coverity.com/projects/31713/badge.svg)

Originally created to be used internally within my [Discrete Fourier Transform Tool](https://github.com/ymich9963/dftt) but ended up being something useful as a standalone tool. Reason for making this is that single channel data is easier to process in certain cases.

To mix the multi-channel data into one channel, the equation below is used,

$$
x_{mono} = \sum_{n = 0}^{N}\sum_{c = 0}^{C} x_{in}[n * C + c] / C 
$$

where,
- $x_{mono}$ is the output buffer with the mono data
- $x_{in}$ is the input buffer of the inputted audio file.
- $C$ is the number of channels present.
- $N$ is the number of samples present in the input buffer.

## Installing
Currently an automatic installation exists only for Windows, and binaries are built only for Windows. For other Operating Systems you need to build from source.

### Windows
To install automatically, use the install script located in `install/` by executing the command below in a PowerShell terminal with Admin rights,

```
irm "https://raw.githubusercontent.com/ymich9963/mix2mono/refs/heads/main/install/mix2mono-windows-install.ps1" | iex
```

The script downloads the executable, moves it to `C:\Program Files\mix2mono\`, and adds that path to the system environment variable. If you do not want the automated script feel free to download the executable or build from source. In case your organisation doesn't allow you to install from the script due to protected paths, download the script and change the `$MIX2MONO_install_path` variable to a location that suits you.

### macOS & Linux
Please the Building section. Use `make` to build from source.

## Usage 
To use `mix2mono` it is done by executing,
```
mix2mono <audio file>
```
With optionally adding a `--info` flag for some more info about the audio file.

The `--help` option is listed below,
```
Mix2Mono options,

Basic usage, `mix2mono <audio file>`.

        -i,     --input <file>  = Path or name of the input file.
        -o,     --output <file> = Path or name of the output file.
                --info          = Output to stdout some info about the input file.
                --version       = Output version number.
```
 
## Building
Only dependency is [libsndfile](https://github.com/libsndfile/libsndfile). Clone the repository and go to this directory. Using `make` simply run in a command line,
```
make
```
And `mix2mono` should be build in a new `build/` directory.

## Tests and Coverage
Running the tests or coverage can be done by running,
```
make test
```
```
make coverage
```
Testing suite used is [Unity](https://github.com/ThrowTheSwitch/Unity) and LLVM-COV for coverage.

