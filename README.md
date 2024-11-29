# neocc

`neocc` is my custom compiler front-end and back-end for the C programming language. 

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

2. Build the project:
   ```bash
   make
   ```

   The compiled binary will be placed in the `bin/` directory as `neocc`.

3. Clean up build files:
   ```bash
   make clean
   ```

## Usage

```bash
neocc <source_file> [-o <output_file>]
```

### Options:
- `<source_file>`: The input source file to compile.
- `-o <output_file>`: (Optional) Specifies the output file for the generated assembly. Defaults to `output.asm` if not provided.

### Examples

1. Compile `example.c` and output to `output.asm` (default):
   ```bash
   ./bin/neocc example.c
   ```

2. Compile `example.c` and specify the output file:
   ```bash
   ./bin/neocc example.c -o custom_output.asm
   ```

## Contributing

Contributions are welcome! Please submit a pull request or open an issue if you have ideas for improvements.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
