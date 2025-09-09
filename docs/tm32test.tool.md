# TM32 Test Framework Specification

## Overview

The TM32 Test Framework (`tm32test`) is a comprehensive testing tool designed specifically for the TM32 virtual CPU ecosystem. It provides advanced testing capabilities including unit testing, integration testing, performance benchmarking, and automated test execution with detailed reporting.

## Features

### Core Testing Capabilities
- **Unit Testing**: Individual component testing with assertion macros
- **Integration Testing**: Cross-component testing and system validation
- **Performance Benchmarking**: Execution time measurement and performance analysis
- **Memory Testing**: Memory leak detection and usage analysis integration
- **Automated Test Discovery**: Automatic detection of test files and test cases

### Advanced Features
- **Test Categorization**: Tag-based test organization and filtering
- **Parallel Test Execution**: Multi-threaded test execution for improved performance
- **Test Result Reporting**: Comprehensive HTML, XML, and JSON report generation
- **Code Coverage Integration**: Coverage analysis when used with appropriate tools
- **Continuous Integration Support**: CI/CD pipeline integration with proper exit codes

## Command Line Interface

### Usage
```bash
tm32test [options] [test-files...]
```

### Options

| Option | Description |
|--------|-------------|
| `--run` | Execute specified tests |
| `--benchmark` | Run performance benchmarks |
| `--report` | Generate test reports |
| `--discovery` | Auto-discover test files |
| `--verbose` | Enable verbose output |
| `--unit` | Run unit tests only |
| `--integration` | Run integration tests only |
| `--all` | Run all tests |
| `--tags <tag1,tag2>` | Run tests with specific tags |
| `--exclude-tags <tag1,tag2>` | Exclude tests with specific tags |
| `--parallel <n>` | Set number of parallel test threads |
| `--timeout <seconds>` | Set test timeout in seconds |
| `--output <format>` | Set output format (console, xml, json, html) |
| `--output-file <file>` | Write results to specified file |
| `--help` | Show help message |
| `--version` | Show version information |

### Examples

```bash
# Run all unit tests with verbose output
tm32test --unit --verbose

# Run specific test files
tm32test --run tests/cpu_tests.tm32test tests/core_tests.tm32test

# Auto-discover and run all tests, generate HTML report
tm32test --discovery --all --report --output html --output-file report.html

# Run benchmarks with parallel execution
tm32test --benchmark --parallel 4

# Run tests with specific tags
tm32test --tags "cpu,memory" --verbose

# Integration testing with timeout
tm32test --integration --timeout 30
```

## Test File Format

### Test Discovery
The framework automatically discovers test files with the following extensions:
- `.tm32test` - TM32-specific test files
- `.cpp` - C++ source files containing test macros
- `.hpp` - C++ header files containing test definitions

### Test Definition Macros

```cpp
#include "TM32TEST/TestFramework.hpp"

// Define a test suite
TM32_TEST_SUITE(CPUTests) {
    
    // Define individual test cases
    TM32_TEST(BasicArithmetic) {
        TM32CPU::CPU cpu;
        // Test implementation
        TM32_ASSERT_EQ(42, cpu.add(20, 22));
    }
    
    TM32_TEST(MemoryOperations, "memory,cpu") {  // With tags
        TM32CORE::Memory memory(1024);
        memory.write(0x100, 0xDEADBEEF);
        TM32_ASSERT_EQ(0xDEADBEEF, memory.read(0x100));
    }
    
    // Benchmark test
    TM32_BENCHMARK(InstructionThroughput) {
        TM32CPU::CPU cpu;
        for (int i = 0; i < 1000000; ++i) {
            cpu.executeInstruction();
        }
    }
}
```

### Assertion Macros

| Macro | Description |
|-------|-------------|
| `TM32_ASSERT(condition, message)` | Basic assertion with custom message |
| `TM32_ASSERT_EQ(expected, actual)` | Equality assertion |
| `TM32_ASSERT_NE(expected, actual)` | Inequality assertion |
| `TM32_ASSERT_LT(a, b)` | Less than assertion |
| `TM32_ASSERT_LE(a, b)` | Less than or equal assertion |
| `TM32_ASSERT_GT(a, b)` | Greater than assertion |
| `TM32_ASSERT_GE(a, b)` | Greater than or equal assertion |
| `TM32_ASSERT_THROWS(expression, exception_type)` | Exception assertion |
| `TM32_ASSERT_NO_THROW(expression)` | No exception assertion |

## Integration with TM32 Components

### TM32 CPU Testing
- Instruction execution validation
- Register state verification
- Pipeline behavior testing
- Edge case handling

### TM32 Core Testing
- Memory subsystem validation
- I/O device simulation testing
- Interrupt handling verification
- Timing accuracy testing

### TM32 Assembler Testing
- Assembly parsing validation
- Object file generation testing
- Optimization verification
- Error handling testing

### TM32 Linker Testing
- Object file linking validation
- Symbol resolution testing
- Library linking verification
- Executable generation testing

### TM32 Emulator Testing
- End-to-end system testing
- Graphics subsystem validation
- Input handling verification
- Performance benchmarking

## Report Generation

### Console Output
- Real-time test progress display
- Color-coded results (pass/fail/skip)
- Summary statistics
- Execution time reporting

### XML Output (JUnit compatible)
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="TM32Tests" tests="150" failures="2" errors="0" time="45.123">
    <testcase classname="CPUTests" name="BasicArithmetic" time="0.001"/>
    <testcase classname="CPUTests" name="MemoryOperations" time="0.005">
        <failure message="Expected: 42, Actual: 24">Test failed</failure>
    </testcase>
</testsuite>
```

### JSON Output
```json
{
    "framework": "TM32 Test Framework",
    "version": "1.0.0",
    "timestamp": "2025-09-08T12:34:56Z",
    "summary": {
        "total": 150,
        "passed": 148,
        "failed": 2,
        "skipped": 0,
        "duration": 45.123
    },
    "suites": [
        {
            "name": "CPUTests",
            "tests": [
                {
                    "name": "BasicArithmetic",
                    "status": "passed",
                    "duration": 0.001
                }
            ]
        }
    ]
}
```

### HTML Output
- Interactive web-based test results
- Sortable test tables
- Test execution graphs
- Failure details with stack traces

## Performance Benchmarking

### Benchmark Metrics
- **Execution Time**: Precise timing measurements
- **Memory Usage**: Peak and average memory consumption
- **CPU Utilization**: Processor usage during test execution
- **Throughput**: Operations per second calculations

### Benchmark Comparison
- Historical performance tracking
- Performance regression detection
- Baseline comparison capabilities
- Performance trend analysis

## Error Handling

The framework uses standardized error codes from the TM32 error handling specification:

| Error Code | Name | Description |
|------------|------|-------------|
| 0x7001 | TEST_EXECUTION_FAILED | Test execution encountered an error |
| 0x7002 | TEST_DISCOVERY_FAILED | Automatic test discovery failed |
| 0x7003 | TEST_REPORT_GENERATION_FAILED | Test report generation failed |
| 0x7004 | BENCHMARK_EXECUTION_FAILED | Performance benchmark execution failed |
| 0x7005 | TEST_CONFIGURATION_INVALID | Test configuration is invalid |
| 0x7006 | TEST_TIMEOUT_EXCEEDED | Test execution exceeded timeout limit |
| 0x7007 | TEST_PARALLEL_EXECUTION_FAILED | Parallel test execution failed |

## Build Integration

### CMake Integration
The test framework integrates seamlessly with the TM32 CMake build system:

```cmake
# Enable testing
enable_testing()

# Add test executable
add_test(NAME tm32_unit_tests COMMAND tm32test --unit)
add_test(NAME tm32_integration_tests COMMAND tm32test --integration)
add_test(NAME tm32_benchmarks COMMAND tm32test --benchmark)
```

### Script Integration
The framework is fully integrated with TM32 build and testing scripts:

```bash
# Run all tests
./scripts/test.sh --all

# Run with test framework
./scripts/test.sh --framework

# Generate coverage report with tests
./scripts/test.sh --coverage --framework
```

## Cross-Platform Support

### Windows
- Native Windows executable
- Visual Studio integration
- Windows-specific memory testing with Application Verifier
- PowerShell script compatibility

### Linux/macOS
- Native Unix executable
- GDB debugging integration
- Valgrind memory testing integration
- Bash script compatibility

## Development Guidelines

### Adding New Tests
1. Create test files in appropriate directories
2. Use descriptive test names and clear assertions
3. Include both positive and negative test cases
4. Add appropriate tags for test categorization
5. Document complex test scenarios

### Performance Considerations
- Minimize test setup/teardown overhead
- Use appropriate timeout values
- Consider memory usage in large test suites
- Optimize for parallel execution where possible

### Best Practices
- Keep tests focused and atomic
- Avoid dependencies between tests
- Use meaningful assertion messages
- Include edge cases and error conditions
- Maintain test documentation

## Future Enhancements

### Planned Features
- Visual test result dashboard
- Test execution recording and playback
- Advanced performance profiling integration
- Distributed test execution support
- Machine learning-based test optimization

### Integration Roadmap
- IDE plugin development (VS Code, CLion)
- Enhanced CI/CD pipeline integration
- Cloud-based test execution support
- Advanced code coverage analysis
- Performance regression detection automation
