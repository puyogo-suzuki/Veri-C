# Veri-C
## Compilation
Install CMake and any c compiler.  
CMake is available on devel/cmake in BSDs.  

Then, execute the following:  
```sh
$ cmake -B build
$ cd build
$ make
```

## Usage
### Parse
```sh
./veri-parse < some_file.txt
```

### Alpha
```sh
./veri-alpha "expression1" "expression2"
```

### Check
```sh
./veri-check < some_file.txt
```
