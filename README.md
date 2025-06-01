
# Vangers Binary Scripts Assembler

### This is a toolset for building and testing scripts of [Vangers](https://github.com/Kranx/Vangers).

I made it because the original game's assembler is pretty slow, and also, you need a modified version of the game to use it.
Many cool things are stored in these scripts, and I want more people to be able editing them.

### Actually, it cannot assemble original game's scripts.

Raw Vangers scripts have an awful structure, like a multiline strings, quoted script keywords, etc.
This is why there is a VBSA Formatter. It solved some problems, but others I had to edit by myself.
All these formatted scripts (and dictionaries for VBSA) are located in the /data/ directory.

## Program usage
VBSA is divides on three main sections: Assembler, Formatter and VVM(Vangers Virtual Machine).

### Assembler options:
 - `-a`, `--assemble <input file name>`: Assemble script file. Requires the output and dictionary to be specified.
 - `-o`, `--output <file name>`: Name of output binary file.
 - `-d`, `--dictionary <file name>`: Load dictionary from file.
 - `-an`, `--analyze <token>`: Analyze token as if it was assembled to the binary script.
 - `-W<error-type>`: Lower the level of error case to warning.

### Formatter options:
 - `-f`, `--format [Target files...]`: Format files to VBSA script standard.
 - `--overwrite-formatted`: Overwrite target files instead of creating new.

### VVM options:
 - `-t`, `--test <target file> <script type: [oftr|aci_hi]>`: Run the binary script through virtual machine which simulates the Vangers script parser.
    You can specify "-" as target file, if you don't want to actually run any script (for example, you just want to open VVM console)
 - `--interactive`: Instead of autorun, VVM console will be opened, where you can track registers, actions, reading history and so on.
 - `--history-size <size>`: Change the size of VVM's actions history.

### Other options:
 - `-h`, `--help`: Print text about VBSA and its options.
 - `-v`, `--version`: Print program's build type, version, and compilation date.
 - `-wl`: Print all available error types for -W option.