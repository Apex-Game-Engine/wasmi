# `wasmi` - Wasm Interpreter

`wasmi` is a WebAssembly Interpreter written from scratch for embedding in C/C++ projects. The primary aim is to build a Wasm scripting runtime for [Apex Game Engine](github.com/Apex-Game-Engine/Apex-Game-Engine)

## Stages
The Wasm runtime has 3 main stages - Decoding, Validation, and Execution

### Decoding
Load Wasm binary format into runtime executable representation
- [ ] Convert ULEB128 and SLEB128 integers into fixed size format
- [ ] Read and decode module sections
  - Types
  - Imports
  - Tags
  - Globals
  - Memory
  - Tables
  - Functions
  - Data
  - Elements
  - Start
  - Exports
- [ ] 

### Validation

### Execution

- Paramteric Instructions
  - [ ] `nop`
  - [ ] `unreachable`
  - [ ] `drop`
  - [ ] `select`
- Control Instructions
  - [ ] `block`
  - [ ] `loop`
  - [ ] `if` `else`
  - [ ] `br`
  - [ ] `br_if`
  - [ ] `br_table`
  - [ ] `br_on_null`
  - [ ] `br_on_non_null`
  - [ ] `br_on_cast`
  - [ ] `br_on_cast_fail`
  - [ ] `call`
  - [ ] `call_ref`
  - [ ] `call_indirect`
  - [ ] `return`
  - [ ] `return_call`
  - [ ] `return_call_ref`
  - [ ] `return_call_indirect`
  - [ ] `throw`
  - [ ] `throw_ref`
  - [ ] `try_table`
- Variable Instructions
  - [ ] `local.get`
  - [ ] `local.set`
  - [ ] `local.tee`
  - [ ] `global.get`
  - [ ] `global.set`
- Table Instructions
  - [ ] `table.get`
  - [ ] `table.set`
  - [ ] `table.size`

## Extensions
