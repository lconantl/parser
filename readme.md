# Project Description

Project Dumper is a fast, lightweight CLI utility built in modern C++ that traverses your project directories and
serializes the codebase into a single, structured Markdown file. Designed to streamline context sharing with Large
Language Models (LLMs), it features intelligent .gitignore rule parsing, automatic binary file exclusion, and a
direct-to-clipboard export option to seamlessly capture your entire codebase context in seconds.

- [Releases](https://github.com/lconantl/parser/releases/) - It is clear that I recommend downloading the latest one.

```
Usage:
  parser [project_path] [options]

Options:
  -h, --help       Show this help and exit
  -t, --tree       Output only the project structure, without file contents
  -c, --clipboard  Copy the result to the clipboard

Examples:
  parser            Collect the current folder into dump.md (will ask for confirmation)
  parser .          Collect the current folder (without confirmation)
  parser -c         Collect the current folder to the clipboard
  parser -t         Output only the project tree
  parser -tc        Project tree directly to the clipboard
  parser -c C:\App  Collect the C:\App project to the clipboard
```