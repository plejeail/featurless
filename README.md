# featurless
Small but useful libraries in C++. If you found a *feature*, please **report it**, it's a **bug**.

The libraries build with cmake.

Featurless libs :
- **<featurless/test.h>**   : a small test framework that does NOT take 1s to compile "hello world"
- **<featurless/log.h>** : a logger that does only one thing, logging

## pre std::format log installation
As long as the std::format library is not available on gcc and clang, the log lib will use fmt library instead. You must download fmt release and extract it in log/lib/fmt.
