# Part 1

## Sqlite
> [Docs about the internals Internals](sqlite.org/arch.html)

### Steps
  * Tokenizer
  * Parser
  * Code Generator
  * Virtual Machine
  * B-Tree
  * Pager
  * OS Interface

### Statements
Non-SQL statements link `.exit` are called "meta-commands". They all start with a dot.

In our code, the `virtual_machine` will be called as `execute_statement`.
