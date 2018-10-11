## SDL Code Edit

Syntax highlighting code edit widget in `SDL`

![Screenshot](coding.png "Screenshot")

This is a simple widget that provides source code editing functionality with basic syntax highlighting. Based on [BalazsJako/ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit). Modified with some changes and improvements. I used the `SDL gfx` library to render primitives including colored rectangles, line numbers, code marks, and the code text per se. You can replace it with other font rasterizer or backend for specific appearance.

### Main features

* Implements typical code editor look and feel
* Supports essential mouse and keyboard work
* Simple automatic indent
* Tab/Shift+Tab to indent/unindent manually
* Ctrl+Z/Ctrl+Y to undo/redo; similar records can be merged
* Customizable language syntax; supports case-insensitive language
* Customizable color palette
* Indicates errors and breakpoints
* Indicates modification of code lines
* Supports exception for multi-line comment
* Supports large files; there is no explicit limit set on file size or number of lines, performance is not affected when large files are loaded (except syntax coloring)

### How to use

The repository contains a Visual Studio solution for Windows. If you were setting up for other platforms or integrating into your own projects:

1. Copy both `code_edit.h` and `code_edit.cpp` in the `/sdl_code_edit` directory to your target environment
2. Copy the `sdl_gfx` library as well for default build
3. See `main.cpp` for usage

### Known issues

* Tooltip is not yet implemented
* Syntax highligthing is based on `std::regex`, which is diasppointingly slow. Because of that, the highlighting process is amortized between multiple frames. Hand-written colorizers and/or a lexical scanner might help resolve this problem
* No variable-width font support
* There's no built-in find/replace support, however it won't be difficult to make it with combination of existing functions
