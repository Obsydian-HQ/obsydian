/**
 * Integration test: Process execution API
 * Tests that the Process API can execute commands and capture output
 * 
 * These tests require a GUI environment on macOS (for NSApplication)
 * but can run in headless CI with proper setup.
 */

#include <gtest/gtest.h>
#include "obsidian/process.h"
#include "obsidian/app.h"
#include <string>
#include <vector>
#include <thread>
#include <chrono>

using namespace obsidian;

class ProcessTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the app for macOS (required for NSTask)
        app.initialize();
    }
    
    void TearDown() override {
        // Cleanup is handled by RAII
    }
    
    App app;
};

TEST_F(ProcessTest, ProcessCreationWorks) {
    Process process;
    
    // Create a simple process (echo command)
    #ifdef __APPLE__
    bool created = process.create("/bin/echo", {"Hello, World!"});
    EXPECT_TRUE(created);
    EXPECT_TRUE(process.isValid());
    #else
    // Other platforms not yet implemented
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessExecutionWorks) {
    Process process;
    
    #ifdef __APPLE__
    bool created = process.create("/bin/echo", {"test"});
    ASSERT_TRUE(created);
    
    bool started = process.start();
    EXPECT_TRUE(started);
    
    // Wait for process to complete
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 0);
    EXPECT_FALSE(process.isRunning());
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessStdoutCallbackWorks) {
    Process process;
    std::string capturedOutput;
    
    #ifdef __APPLE__
    bool created = process.create("/bin/echo", {"Hello from process"});
    ASSERT_TRUE(created);
    
    // Set up stdout callback
    process.setOnStdout([&capturedOutput](const std::string& data) {
        capturedOutput += data;
    });
    
    bool started = process.start();
    ASSERT_TRUE(started);
    
    // Wait for process to complete
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 0);
    
    // Check that output was captured
    EXPECT_NE(capturedOutput.find("Hello from process"), std::string::npos);
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessStderrCallbackWorks) {
    Process process;
    std::string capturedError;
    
    #ifdef __APPLE__
    // Use a command that writes to stderr
    // /bin/sh -c "echo 'error message' >&2" writes to stderr
    bool created = process.create("/bin/sh", {"-c", "echo 'error message' >&2"});
    ASSERT_TRUE(created);
    
    // Set up stderr callback
    process.setOnStderr([&capturedError](const std::string& data) {
        capturedError += data;
    });
    
    bool started = process.start();
    ASSERT_TRUE(started);
    
    // Wait for process to complete
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 0);
    
    // Check that error output was captured
    EXPECT_NE(capturedError.find("error message"), std::string::npos);
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessTerminationCallbackWorks) {
    Process process;
    int capturedExitCode = -1;
    bool terminationCalled = false;
    
    #ifdef __APPLE__
    bool created = process.create("/bin/echo", {"test"});
    ASSERT_TRUE(created);
    
    // Set up termination callback
    process.setOnTermination([&capturedExitCode, &terminationCalled](int exitCode) {
        capturedExitCode = exitCode;
        terminationCalled = true;
    });
    
    bool started = process.start();
    ASSERT_TRUE(started);
    
    // Wait for process to complete
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 0);
    
    // Give callbacks time to execute (they're called asynchronously)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that termination callback was called
    // Note: On macOS, termination callback may be called asynchronously
    // so we check the exit code directly
    EXPECT_EQ(capturedExitCode, 0);
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessTerminateWorks) {
    Process process;
    
    #ifdef __APPLE__
    // Start a long-running process (sleep)
    bool created = process.create("/bin/sleep", {"2"});
    ASSERT_TRUE(created);
    
    bool started = process.start();
    ASSERT_TRUE(started);
    EXPECT_TRUE(process.isRunning());
    
    // Terminate the process
    process.terminate();
    
    // Wait a bit for termination
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Process should no longer be running
    EXPECT_FALSE(process.isRunning());
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessGetExitCodeWorks) {
    Process process;
    
    #ifdef __APPLE__
    // Use a command that exits with a specific code
    // /bin/sh -c "exit 42" exits with code 42
    bool created = process.create("/bin/sh", {"-c", "exit 42"});
    ASSERT_TRUE(created);
    
    bool started = process.start();
    ASSERT_TRUE(started);
    
    // Wait for process to complete
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 42);
    
    // Get exit code again
    int exitCode2 = process.getExitCode();
    EXPECT_EQ(exitCode2, 42);
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

TEST_F(ProcessTest, ProcessWithWorkingDirectoryWorks) {
    Process process;
    std::string capturedOutput;
    
    #ifdef __APPLE__
    // Use pwd to verify working directory
    bool created = process.create("/bin/pwd", {}, "/tmp");
    ASSERT_TRUE(created);
    
    process.setOnStdout([&capturedOutput](const std::string& data) {
        capturedOutput += data;
    });
    
    bool started = process.start();
    ASSERT_TRUE(started);
    
    int exitCode = process.waitUntilExit();
    EXPECT_EQ(exitCode, 0);
    
    // Check that output contains /tmp
    EXPECT_NE(capturedOutput.find("/tmp"), std::string::npos);
    #else
    GTEST_SKIP() << "Process API not implemented on this platform";
    #endif
}

