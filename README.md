# alui

UI library for Allegro inspired by [Agui](https://github.com/jmasterx/Agui/).

This is a WIP library that I'll be poking at on and off for the foreseeable future.

## Notable differences from agui

* Modern C++; smart pointers; reduced number of raw pointers; better use of RAII; hopefully better to work with
* alui is allegro-specific (hence the `al` in the name, the prefix commonly used in `allegro` functions), while Agui is renderer-agnostic. No other renderers will be supported, and this is an intentional decision. This avoids unnecessary duplicate objects (`ALLEGRO_FONT` and `agui::Font`, `ALLEGRO_COLOR` and `agui::Color`), which means less code to write and no duplicates at runtime (mainly affects fonts and similar). `ALLEGRO_COLOR` is a bit lower-level to work with than a custom class, but it's consistent, and I like consistent.

## Requirements
* C++17
* allegro, with `allegro_font`, `allegro_color`, `allegro_primitives`.
    * There are additional requirements if running the demos (which you won't if you use this library correctly), but allegro is automatically built if the demos are built, so this is a non-issue unless you have some weird fucking system config

## Using the library

### Recommended: `FetchContent`

```cmake
include(FetchContent)

# TODO: later
```


## License

MIT. See the LICENSE file.
