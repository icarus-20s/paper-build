# Question Paper System

A Qt-based application for creating and managing question papers with support for PDF and DOCX export.

## Features

- Create and edit question papers with multiple sections
- Support for multiple question types (MCQ, descriptive)
- Live preview of question papers
- Export to PDF and DOCX formats
- Print support
- Modern Qt-based GUI

## Prerequisites

### Required Software

- **Qt6** (version 6.0 or later)
  - Qt6 Widgets
  - Qt6 Core
  - Qt6 GUI
  - Qt6 Print Support
- **CMake** (version 3.16 or later)
- **C++ Compiler** with C++17 support
  - GCC 7+ (Linux)
  - Clang 5+ (Linux/macOS)
  - MSVC 2017+ (Windows)

### Installing Qt6

#### Linux (Ubuntu/Debian)
```bash
# Install Qt6 development packages
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

#### Windows
1. Download Qt Creator from [qt.io](https://www.qt.io/download)
2. Install Qt6 with the online installer
3. Make sure CMake is in your PATH

#### macOS
```bash
# Install Qt6 using Homebrew
brew install qt6 cmake
```

## Building the Project

### Quick Build (Recommended)

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd question_paper
   ```

2. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   cmake --build .
   ```

5. **Run the application:**
   ```bash
   ./bin/question_paper_system
   ```

### Alternative Build Methods

#### Using Ninja (Faster builds)
```bash
cd build
cmake .. -G Ninja
ninja
```

#### Debug Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

#### Release Build
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### Parallel Build (Use all CPU cores)
```bash
cd build
cmake ..
cmake --build . -- -j$(nproc)
```

## Project Structure

```
question_paper/
├── CMakeLists.txt          # CMake build configuration
├── src/                    # Source code
│   ├── app/               # Main application
│   ├── models/            # Data models
│   ├── exporters/         # Export functionality
│   ├── dialogs/           # Dialog windows
│   ├── widgets/           # Custom UI widgets
│   ├── pages/             # Application pages
│   └── utils/             # Utility functions
├── tests/                  # Unit tests
├── assets/                 # Application assets
├── data/                   # Data files
├── resources.qrc          # Qt resource file
└── build/                 # Build directory (generated)
```

## Dependencies

The project uses the following Qt6 modules:
- `Qt6::Widgets` - GUI components
- `Qt6::Core` - Core functionality
- `Qt6::Gui` - Graphics and GUI base
- `Qt6::PrintSupport` - Printing support

## Testing

The project includes unit tests that can be run after building:

```bash
cd build
ctest
# or
./layout_test
```

## Export Features

The application supports exporting question papers to:
- **PDF** - Using Qt's printing system
- **DOCX** - HTML-based export (opens in Word)

## Troubleshooting

### Common Build Issues

1. **Qt6 not found:**
   ```bash
   # Set Qt6 path explicitly
   export CMAKE_PREFIX_PATH=/path/to/qt6
   cmake ..
   ```

2. **CMake version too old:**
   ```bash
   # Install newer CMake
   sudo apt install cmake  # Ubuntu/Debian
   # or download from cmake.org
   ```

3. **Missing C++17 support:**
   - Update your compiler
   - GCC 7+ or Clang 5+ required

4. **Qt Creator issues:**
   - Make sure Qt6 kit is configured
   - Check that CMake is properly configured

### Runtime Issues

1. **Application won't start:**
   - Check that all Qt6 libraries are installed
   - Ensure the executable has proper permissions

2. **Export not working:**
   - Check write permissions for output directories
   - Ensure Qt Print Support is installed

## Development

### Code Style
- C++17 standard
- Qt coding conventions
- CMake-based build system
- Unit tests for critical functionality

### Adding New Features
1. Follow the existing code structure
2. Add appropriate unit tests
3. Update CMakeLists.txt if adding new files
4. Test on multiple platforms if possible

## Support

For issues and questions:
1. Check the troubleshooting section above
2. Review the code comments for API documentation
3. Create an issue in the project repository</content>