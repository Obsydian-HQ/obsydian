/**
 * macOS Process List FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSWorkspace and NSRunningApplication APIs
 */

#import "macos_process_list.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <signal.h>
#import <memory>
#import <vector>
#import <string>

// Helper function to convert NSString to C string (allocated, must be freed)
static char* copyCString(NSString* str) {
    if (!str) {
        return nullptr;
    }
    const char* utf8 = [str UTF8String];
    if (!utf8) {
        return nullptr;
    }
    size_t len = strlen(utf8);
    char* result = (char*)malloc(len + 1);
    if (result) {
        strcpy(result, utf8);
    }
    return result;
}

// Helper function to free process list
static void freeProcessList(ObsidianProcessList* processList) {
    if (!processList) {
        return;
    }
    
    // Free all process info strings
    if (processList->processes) {
        for (int i = 0; i < processList->count; i++) {
            if (processList->processes[i].processName) {
                free((void*)processList->processes[i].processName);
            }
            if (processList->processes[i].bundleIdentifier) {
                free((void*)processList->processes[i].bundleIdentifier);
            }
        }
        free(processList->processes);
    }
    
    free(processList);
}

extern "C" {

ObsidianProcessList* obsidian_macos_get_all_processes() {
    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSArray<NSRunningApplication*>* runningApps = [workspace runningApplications];
    
    // Allocate process list structure
    ObsidianProcessList* processList = (ObsidianProcessList*)malloc(sizeof(ObsidianProcessList));
    if (!processList) {
        return nullptr;
    }
    
    processList->count = static_cast<int>([runningApps count]);
    processList->processes = nullptr;
    
    if (processList->count == 0) {
        return processList;
    }
    
    // Allocate array of process info
    processList->processes = (ObsidianProcessInfo*)malloc(sizeof(ObsidianProcessInfo) * processList->count);
    if (!processList->processes) {
        free(processList);
        return nullptr;
    }
    
    // Fill in process information
    for (int i = 0; i < processList->count; i++) {
        NSRunningApplication* app = [runningApps objectAtIndex:i];
        ObsidianProcessInfo* info = &processList->processes[i];
        
        info->processId = [app processIdentifier];
        
        NSString* processName = [app localizedName];
        if (!processName) {
            processName = [app bundleIdentifier];
        }
        info->processName = copyCString(processName);
        
        NSString* bundleId = [app bundleIdentifier];
        info->bundleIdentifier = copyCString(bundleId);
        
        info->isActive = [app isActive];
    }
    
    return processList;
}

void obsidian_macos_process_list_free(ObsidianProcessList* processList) {
    freeProcessList(processList);
}

bool obsidian_macos_kill_process(int32_t processId) {
    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSArray<NSRunningApplication*>* runningApps = [workspace runningApplications];
    
    // Find the process by PID
    for (NSRunningApplication* app in runningApps) {
        if ([app processIdentifier] == processId) {
            // Terminate the application
            BOOL success = [app terminate];
            return success == YES;
        }
    }
    
    // If not found in running applications, try using kill() system call
    // Note: This requires appropriate permissions
    int result = kill(processId, SIGTERM);
    return result == 0;
}

bool obsidian_macos_get_process_info(int32_t processId, ObsidianProcessInfo* processInfo) {
    if (!processInfo) {
        return false;
    }
    
    // Initialize output structure
    processInfo->processId = -1;
    processInfo->processName = nullptr;
    processInfo->bundleIdentifier = nullptr;
    processInfo->isActive = false;
    
    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSArray<NSRunningApplication*>* runningApps = [workspace runningApplications];
    
    // Find the process by PID
    for (NSRunningApplication* app in runningApps) {
        if ([app processIdentifier] == processId) {
            processInfo->processId = processId;
            
            NSString* processName = [app localizedName];
            if (!processName) {
                processName = [app bundleIdentifier];
            }
            processInfo->processName = copyCString(processName);
            
            NSString* bundleId = [app bundleIdentifier];
            processInfo->bundleIdentifier = copyCString(bundleId);
            
            processInfo->isActive = [app isActive];
            
            return true;
        }
    }
    
    return false;
}

} // extern "C"

