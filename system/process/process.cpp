/**
 * Obsidian Public API - Process Implementation
 * 
 * This file implements the public Process API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/system/process.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_process.h"  // For C FFI function
#endif

namespace obsidian {

class Process::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::Process macosProcess;
#endif
    bool valid = false;
};

Process::Process() : pImpl(std::make_unique<Impl>()) {}

Process::~Process() {
    // RAII: Automatically clean up resources
    // The FFI Process destructor will handle proper cleanup (terminate + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI Process
    // The FFI Process destructor calls obsidian_macos_destroy_process which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool Process::create(const std::string& command, 
                    const std::vector<std::string>& arguments,
                    const std::string& workingDirectory) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
    if (command.empty()) {
        return false;
    }
    
#ifdef __APPLE__
    if (!pImpl->macosProcess.create(command, arguments, workingDirectory)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

bool Process::start() {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.start();
#else
    return false;
#endif
}

void Process::setOnStdout(std::function<void(const std::string&)> callback) {
    // Allow setting callbacks before create() - they will be registered when create() is called
#ifdef __APPLE__
    pImpl->macosProcess.setOnStdout(callback);
#endif
}

void Process::setOnStderr(std::function<void(const std::string&)> callback) {
    // Allow setting callbacks before create() - they will be registered when create() is called
#ifdef __APPLE__
    pImpl->macosProcess.setOnStderr(callback);
#endif
}

void Process::setOnTermination(std::function<void(int)> callback) {
    // Allow setting callbacks before create() - they will be registered when create() is called
#ifdef __APPLE__
    pImpl->macosProcess.setOnTermination(callback);
#endif
}

void Process::setOnError(std::function<void(const std::string&)> callback) {
    // Allow setting callbacks before create() - they will be registered when create() is called
#ifdef __APPLE__
    pImpl->macosProcess.setOnError(callback);
#endif
}

void Process::terminate() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosProcess.terminate();
#endif
}

bool Process::isRunning() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.isRunning();
#else
    return false;
#endif
}

int Process::waitUntilExit() {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.waitUntilExit();
#else
    return -1;
#endif
}

int Process::getExitCode() const {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.getExitCode();
#else
    return -1;
#endif
}

int32_t Process::getPid() const {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.getPid();
#else
    return -1;
#endif
}

bool Process::isValid() const {
    if (!pImpl) {
        return false;
    }
    
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosProcess.isValid();
#else
    return false;
#endif
}

Process::Process(Process&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Process& Process::operator=(Process&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

} // namespace obsidian

