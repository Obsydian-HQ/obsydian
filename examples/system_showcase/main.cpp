/**
 * System APIs Showcase Example Application
 * 
 * This example demonstrates the Process execution and Process management APIs.
 * It provides a GUI interface to test these APIs manually.
 * 
 * Features:
 * - Execute shell commands and view output
 * - List all running processes
 * - View process details
 * - Terminate processes (with confirmation)
 * 
 * This example uses the public Obsidian API - exactly how real users
 * (like Glass) will use Obsidian.
 */

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <obsidian/obsidian.h>

using namespace obsidian;

// Global state for the example
struct AppState {
    Window* window = nullptr;
    TextField* commandInput = nullptr;
    TextView* outputView = nullptr;
    Table* processTable = nullptr;
    Button* executeButton = nullptr;
    Button* refreshButton = nullptr;
    Button* killButton = nullptr;
    std::vector<ProcessInfo> currentProcesses;
    int selectedProcessIndex = -1;
};

AppState g_state;

void updateProcessList() {
    if (!g_state.processTable) {
        return;
    }
    
    // Clear existing rows
    g_state.processTable->clear();
    
    // Get all processes
    g_state.currentProcesses = ProcessList::getAllProcesses();
    
    // Add processes to table
    for (const auto& process : g_state.currentProcesses) {
        std::vector<std::string> rowData = {
            std::to_string(process.processId),
            process.processName,
            process.bundleIdentifier.empty() ? "(none)" : process.bundleIdentifier,
            process.isActive ? "Yes" : "No"
        };
        g_state.processTable->addRow(rowData);
    }
    
}

void executeCommand() {
    if (!g_state.commandInput || !g_state.outputView) {
        return;
    }
    
    std::string command = g_state.commandInput->getText();
    if (command.empty()) {
        return;
    }
    
    // Clear output
    g_state.outputView->setString("");
    
    // Create process
    Process process;
    
    // Parse command - if it doesn't start with /, use shell execution
    std::vector<std::string> args;
    std::string cmdPath;
    
    // Check if command starts with / (absolute path)
    if (command.find('/') == 0) {
        // Absolute path - parse as command + arguments
        std::istringstream iss(command);
        std::string token;
        if (iss >> cmdPath) {
            while (iss >> token) {
                args.push_back(token);
            }
        }
    } else {
        cmdPath = "/bin/sh";
        args = {"-c", command};
    }
    
    // Collect output - use shared pointer to ensure thread safety
    auto outputPtr = std::make_shared<std::string>();
    
    process.setOnStdout([outputPtr](const std::string& data) {
        *outputPtr += data;
        if (g_state.outputView) {
            g_state.outputView->setString(*outputPtr);
        }
    });
    
    process.setOnStderr([outputPtr](const std::string& data) {
        *outputPtr += "[ERROR] " + data;
        if (g_state.outputView) {
            g_state.outputView->setString(*outputPtr);
        }
    });
    
    process.setOnTermination([outputPtr](int exitCode) {
        *outputPtr += "\n[Process exited with code: " + std::to_string(exitCode) + "]\n";
        if (g_state.outputView) {
            g_state.outputView->setString(*outputPtr);
        }
    });
    
    process.setOnError([outputPtr](const std::string& errorMessage) {
        *outputPtr += "[ERROR] " + errorMessage + "\n";
        if (g_state.outputView) {
            g_state.outputView->setString(*outputPtr);
        }
    });
    
    if (!process.create(cmdPath, args)) {
        std::string errorMsg = "[ERROR] Failed to create process for: " + cmdPath + "\n";
        if (g_state.outputView) {
            g_state.outputView->setString(errorMsg);
        }
        return;
    }
    
    if (!process.start()) {
        std::string errorMsg = "[ERROR] Failed to start process. Check error callback for details.\n";
        *outputPtr += errorMsg;
        if (g_state.outputView) {
            g_state.outputView->setString(*outputPtr);
        }
        return;
    }
    
    process.waitUntilExit();
    
    auto startTime = std::chrono::steady_clock::now();
    size_t lastOutputSize = outputPtr->length();
    const auto maxWaitTime = std::chrono::milliseconds(500);
    
    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        if (elapsed > maxWaitTime) {
            break;
        }
        
        size_t currentOutputSize = outputPtr->length();
        if (currentOutputSize > lastOutputSize) {
            lastOutputSize = currentOutputSize;
            startTime = std::chrono::steady_clock::now();
        }
        
        if (currentOutputSize > 0) {
            auto timeSinceLastData = std::chrono::steady_clock::now() - startTime;
            if (timeSinceLastData > std::chrono::milliseconds(100)) {
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    if (g_state.outputView) {
        g_state.outputView->setString(*outputPtr);
    }
}

void onProcessSelection(int rowIndex) {
    g_state.selectedProcessIndex = rowIndex;
    
    if (g_state.killButton && rowIndex >= 0 && rowIndex < static_cast<int>(g_state.currentProcesses.size())) {
        g_state.killButton->setEnabled(true);
    }
}

void killSelectedProcess() {
    if (g_state.selectedProcessIndex < 0 || 
        g_state.selectedProcessIndex >= static_cast<int>(g_state.currentProcesses.size())) {
        return;
    }
    
    const auto& process = g_state.currentProcesses[g_state.selectedProcessIndex];
    
    if (ProcessList::killProcess(process.processId)) {
        updateProcessList();
        g_state.selectedProcessIndex = -1;
        if (g_state.killButton) {
            g_state.killButton->setEnabled(false);
        }
    }
}

int main(int /* argc */, char* /* argv */[]) {
    App app;
    
    if (!app.initialize()) {
        return 1;
    }
    
    Window window;
    if (!window.create(1000, 700, "System APIs Showcase")) {
        app.shutdown();
        return 1;
    }
    
    g_state.window = &window;
    
    TextField commandInput;
    if (commandInput.create("Enter command (e.g., 'ls -la' or '/bin/echo hello')", 20, 650, 600, 30)) {
        commandInput.addToWindow(window);
        g_state.commandInput = &commandInput;
    }
    
    Button executeButton;
    if (executeButton.create("Execute", 640, 650, 100, 30)) {
        executeButton.addToWindow(window);
        executeButton.setOnClick(executeCommand);
        g_state.executeButton = &executeButton;
    }
    
    ScrollView outputScrollView;
    TextView outputView;
    if (outputScrollView.create(20, 400, 960, 230)) {
        if (outputView.create(0, 0, 960, 230)) {
            outputView.setEditable(false);
            outputView.setSelectable(true);
            outputView.setString("Command output will appear here...\n");
            
            outputScrollView.setDocumentView(outputView);
            outputScrollView.addToWindow(window);
            g_state.outputView = &outputView;
        }
    }
    
    Table processTable;
    if (processTable.create(20, 20, 960, 360)) {
        processTable.addColumn("pid", "PID", 80);
        processTable.addColumn("name", "Process Name", 200);
        processTable.addColumn("bundle", "Bundle ID", 300);
        processTable.addColumn("active", "Active", 80);
        processTable.addToWindow(window);
        processTable.setOnSelection(onProcessSelection);
        g_state.processTable = &processTable;
    }
    
    Button refreshButton;
    if (refreshButton.create("Refresh", 880, 400, 100, 30)) {
        refreshButton.addToWindow(window);
        refreshButton.setOnClick([]() {
            updateProcessList();
        });
        g_state.refreshButton = &refreshButton;
    }
    
    Button killButton;
    if (killButton.create("Kill Process", 880, 440, 100, 30)) {
        killButton.addToWindow(window);
        killButton.setEnabled(false);
        killButton.setOnClick(killSelectedProcess);
        g_state.killButton = &killButton;
    }
    
    updateProcessList();
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}

