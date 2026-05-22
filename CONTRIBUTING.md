# Contributing to XML_Lib

Thank you for your interest in contributing to XML_Lib! We welcome bug reports, feature requests, and pull requests.

## How to Contribute

1. **Fork the repository** and create a new branch for your feature or bugfix.
2. **Write clear, concise commit messages** and keep changes focused and atomic.
3. **Follow the coding style** used in the project (modern C++23 idioms, consistent formatting).
4. **Add or update tests** for any new features or bugfixes.
5. **Ensure all tests pass** and the project builds cleanly with no warnings (use `-DXML_WARNINGS_AS_ERRORS=ON`).
6. **Document your changes** in the relevant markdown files (API, Guide, Package, etc.).
7. **Submit a pull request** with a clear description of your changes and the motivation behind them.

## Code of Conduct

Please be respectful and constructive in all interactions.

## Development Notes

- Public API is limited to the headers in `classes/include/` (see `README.md`).
- Internal implementation details are not part of the public API and may change.
- Use CMake options to enable/disable features and build profiles (see `docs/Package.md`).
- Run all tests before submitting (`ctest` or `cmake --build . --target test`).
- For major changes, open an issue or discussion first.

## Contact

For questions or help, open an issue or discussion on the repository.
