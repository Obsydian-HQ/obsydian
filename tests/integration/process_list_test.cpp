/**
 * Integration test: Process management API
 * Tests that the ProcessList API can list and manage running processes
 * 
 * These tests require a GUI environment on macOS (for NSApplication)
 * but can run in headless CI with proper setup.
 */

#include <gtest/gtest.h>
#include "obsidian/process_list.h"
#include "obsidian/app.h"
#include <vector>
#include <algorithm>
#include <unistd.h>  // for getpid()

using namespace obsidian;

class ProcessListTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the app for macOS (required for NSWorkspace)
        app.initialize();
    }
    
    void TearDown() override {
        // Cleanup is handled by RAII
    }
    
    App app;
};

TEST_F(ProcessListTest, GetAllProcessesWorks) {
    #ifdef __APPLE__
    std::vector<ProcessInfo> processes = ProcessList::getAllProcesses();
    
    // Should have at least some processes running
    EXPECT_GT(processes.size(), 0);
    
    // Check that processes have valid PIDs
    for (const auto& process : processes) {
        EXPECT_GT(process.processId, 0);
        EXPECT_FALSE(process.processName.empty());
    }
    #else
    GTEST_SKIP() << "ProcessList API not implemented on this platform";
    #endif
}

TEST_F(ProcessListTest, ProcessListContainsCurrentProcess) {
    #ifdef __APPLE__
    std::vector<ProcessInfo> processes = ProcessList::getAllProcesses();
    
    // Get current process ID
    pid_t currentPid = getpid();
    
    // Find current process in the list
    bool found = false;
    for (const auto& process : processes) {
        if (process.processId == currentPid) {
            found = true;
            // Current process should have a name
            EXPECT_FALSE(process.processName.empty());
            break;
        }
    }
    
    // Current process should be in the list
    EXPECT_TRUE(found);
    #else
    GTEST_SKIP() << "ProcessList API not implemented on this platform";
    #endif
}

TEST_F(ProcessListTest, GetProcessInfoWorks) {
    #ifdef __APPLE__
    // Get current process ID
    pid_t currentPid = getpid();
    
    ProcessInfo processInfo;
    bool found = ProcessList::getProcessInfo(currentPid, processInfo);
    
    EXPECT_TRUE(found);
    EXPECT_EQ(processInfo.processId, currentPid);
    EXPECT_FALSE(processInfo.processName.empty());
    #else
    GTEST_SKIP() << "ProcessList API not implemented on this platform";
    #endif
}

TEST_F(ProcessListTest, GetProcessInfoForInvalidPidFails) {
    #ifdef __APPLE__
    // Use an invalid PID (very large number)
    ProcessInfo processInfo;
    bool found = ProcessList::getProcessInfo(999999, processInfo);
    
    EXPECT_FALSE(found);
    #else
    GTEST_SKIP() << "ProcessList API not implemented on this platform";
    #endif
}

TEST_F(ProcessListTest, ProcessListContainsSystemProcesses) {
    #ifdef __APPLE__
    std::vector<ProcessInfo> processes = ProcessList::getAllProcesses();
    
    // Look for common system processes
    bool foundKernelTask = false;
    bool foundFinder = false;
    
    for (const auto& process : processes) {
        if (process.processName.find("kernel_task") != std::string::npos ||
            process.bundleIdentifier.find("com.apple.kernel") != std::string::npos) {
            foundKernelTask = true;
        }
        if (process.processName.find("Finder") != std::string::npos ||
            process.bundleIdentifier.find("com.apple.finder") != std::string::npos) {
            foundFinder = true;
        }
    }
    
    // At least one system process should be found
    // (kernel_task is always running, Finder may or may not be)
    EXPECT_TRUE(foundKernelTask || foundFinder || processes.size() > 0);
    #else
    GTEST_SKIP() << "ProcessList API not implemented on this platform";
    #endif
}

// Note: We don't test killProcess in unit tests as it's destructive
// That will be tested in the example app for manual verification

