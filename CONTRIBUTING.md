# Contribution guidelines

## Basic guidelines

### Use of generative AI is banned

Generative AI uses training data [based on plagiarism and piracy](https://web.archive.org/web/20250000000000*/https://www.theatlantic.com/technology/archive/2025/03/libgen-meta-openai/682093/), has [significant environmental costs associated with it](https://doi.org/10.21428/e4baedd9.9070dfe7), and [generates fundamentally insecure code](https://doi.org/10.1007/s10664-024-10590-1). GenAI is not ethically built, ethical to use, nor safe to use for programming applications. When caught, you will be permanently banned from contributing to the project, and any prior contributions will be checked and potentially reverted. Any and all contributions you've made cannot be trusted if AI slop machines were involved.

## Development setup

You'll need Conan installed and accessible in your PATH before doing the setup. You can install conan with pip: `pip3 install conan`.
Then, after cloning, run:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DALUI_LINT=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j $(nproc)
```

And you're good to go. `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` is optional, but if you're using an LSP and an editor that doesn't deal with that for you, you'll want it.

### Running tests
Tests are wired up as a custom CMake command. You can run it like you'd run any other command:
```
make -j $(nproc) test
```

> [!note]
>
> If you're on Linux, it's recommended you `xvfb-run` the make command. The test setup spawns and closes many windows in quick succession, which is annoying to look at and functionally prevents you from doing anything else in parallel. `xvfb-run` is not a requirement to run the tests, but it makes running the tests a lot nicer.

