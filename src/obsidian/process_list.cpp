/**
 * Obsidian Public API - ProcessList Implementation
 * 
 * This file implements the public ProcessList API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/process_list.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_process_list.h"  // For C FFI function
#endif

namespace obsidian {

std::vector<ProcessInfo> ProcessList::getAllProcesses() {
#ifdef __APPLE__
    std::vector<obsidian::ffi::macos::ProcessInfo> ffiProcesses = 
        obsidian::ffi::macos::ProcessList::getAllProcesses();
    
    std::vector<ProcessInfo> result;
    result.reserve(ffiProcesses.size());
    
    for (const auto& ffiProcess : ffiProcesses) {
        ProcessInfo info;
        info.processId = ffiProcess.processId;
        info.processName = ffiProcess.processName;
        info.bundleIdentifier = ffiProcess.bundleIdentifier;
        info.isActive = ffiProcess.isActive;
        result.push_back(info);
    }
    
    return result;
#else
    // Other platforms not yet implemented
    return std::vector<ProcessInfo>();
#endif
}

bool ProcessList::killProcess(int32_t processId) {
#ifdef __APPLE__
    return obsidian::ffi::macos::ProcessList::killProcess(processId);
#else
    // Other platforms not yet implemented
    return false;
#endif
}

bool ProcessList::getProcessInfo(int32_t processId, ProcessInfo& processInfo) {
#ifdef __APPLE__
    obsidian::ffi::macos::ProcessInfo ffiInfo;
    if (!obsidian::ffi::macos::ProcessList::getProcessInfo(processId, ffiInfo)) {
        return false;
    }
    
    processInfo.processId = ffiInfo.processId;
    processInfo.processName = ffiInfo.processName;
    processInfo.bundleIdentifier = ffiInfo.bundleIdentifier;
    processInfo.isActive = ffiInfo.isActive;
    
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

} // namespace obsidian

