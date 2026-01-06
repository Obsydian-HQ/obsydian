/**
 * macOS Process FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSTask APIs
 */

#import "macos_process.h"
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <dispatch/dispatch.h>
#import <memory>
#import <vector>
#import <cstring>

// Internal process wrapper class
@interface ObsidianProcessWrapper : NSObject {
    NSTask* _task;
    NSPipe* _stdoutPipe;
    NSPipe* _stderrPipe;
    NSFileHandle* _stdoutHandle;
    NSFileHandle* _stderrHandle;
    ObsidianProcessOutputCallback _stdoutCallback;
    ObsidianProcessOutputCallback _stderrCallback;
    ObsidianProcessTerminationCallback _terminationCallback;
    void* _stdoutUserData;
    void* _stderrUserData;
    void* _terminationUserData;
    int _exitCode;
    bool _isRunning;
    bool _stdoutEOF;
    bool _stderrEOF;
    bool _terminationCallbackCalled;
}

@property (nonatomic, strong) NSTask* task;
@property (nonatomic, strong) NSPipe* stdoutPipe;
@property (nonatomic, strong) NSPipe* stderrPipe;
@property (nonatomic, strong) NSFileHandle* stdoutHandle;
@property (nonatomic, strong) NSFileHandle* stderrHandle;
@property (nonatomic, assign) ObsidianProcessOutputCallback stdoutCallback;
@property (nonatomic, assign) ObsidianProcessOutputCallback stderrCallback;
@property (nonatomic, assign) ObsidianProcessTerminationCallback terminationCallback;
@property (nonatomic, assign) ObsidianProcessErrorCallback errorCallback;
@property (nonatomic, assign) void* stdoutUserData;
@property (nonatomic, assign) void* stderrUserData;
@property (nonatomic, assign) void* terminationUserData;
@property (nonatomic, assign) void* errorUserData;
@property (nonatomic, assign) int exitCode;
@property (nonatomic, assign) bool isRunning;
@property (nonatomic, assign) bool stdoutEOF;
@property (nonatomic, assign) bool stderrEOF;
@property (nonatomic, assign) bool terminationCallbackCalled;

- (instancetype)initWithParams:(ObsidianProcessParams)params;
- (bool)start;
- (void)setOnStdout:(ObsidianProcessOutputCallback)callback userData:(void*)userData;
- (void)setOnStderr:(ObsidianProcessOutputCallback)callback userData:(void*)userData;
- (void)setOnTermination:(ObsidianProcessTerminationCallback)callback userData:(void*)userData;
- (void)setOnError:(ObsidianProcessErrorCallback)callback userData:(void*)userData;
- (void)terminate;
- (bool)isRunning;
- (int)waitUntilExit;
- (int)getExitCode;
- (int32_t)getProcessIdentifier;
- (void)handleStdoutData:(NSNotification*)notification;
- (void)handleStderrData:(NSNotification*)notification;
- (void)handleTermination:(NSNotification*)notification;
- (void)checkAndCleanup;

@end

@implementation ObsidianProcessWrapper

- (instancetype)initWithParams:(ObsidianProcessParams)params {
    self = [super init];
    if (self) {
        // Create NSTask
        _task = [[NSTask alloc] init];
        
        // Set launch path (command)
        if (params.command) {
            NSString* commandStr = [NSString stringWithUTF8String:params.command];
            [_task setLaunchPath:commandStr];
        } else {
            return nil; // Command is required
        }
        
        // Set arguments
        if (params.arguments && params.argumentCount > 0) {
            NSMutableArray* args = [NSMutableArray array];
            for (int i = 0; i < params.argumentCount; i++) {
                if (params.arguments[i]) {
                    NSString* argStr = [NSString stringWithUTF8String:params.arguments[i]];
                    [args addObject:argStr];
                }
            }
            [_task setArguments:args];
        }
        
        // Set working directory if provided
        if (params.workingDirectory) {
            NSString* workingDirStr = [NSString stringWithUTF8String:params.workingDirectory];
            [_task setCurrentDirectoryPath:workingDirStr];
        }
        
        // Create pipes for stdout and stderr
        _stdoutPipe = [NSPipe pipe];
        _stderrPipe = [NSPipe pipe];
        
        // Set standard output and error to pipes
        [_task setStandardOutput:_stdoutPipe];
        [_task setStandardError:_stderrPipe];
        
        // Get file handles for reading
        _stdoutHandle = [_stdoutPipe fileHandleForReading];
        _stderrHandle = [_stderrPipe fileHandleForReading];
        
        // Set up notification observers for data availability
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleStdoutData:)
                                                     name:NSFileHandleReadCompletionNotification
                                                   object:_stdoutHandle];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleStderrData:)
                                                     name:NSFileHandleReadCompletionNotification
                                                   object:_stderrHandle];
        
        // Set up notification observer for process termination
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleTermination:)
                                                     name:NSTaskDidTerminateNotification
                                                   object:_task];
        
        // Initialize callbacks
        _stdoutCallback = nullptr;
        _stderrCallback = nullptr;
        _terminationCallback = nullptr;
        _errorCallback = nullptr;
        _stdoutUserData = nullptr;
        _stderrUserData = nullptr;
        _terminationUserData = nullptr;
        _errorUserData = nullptr;
        _exitCode = -1;
        _isRunning = false;
        _stdoutEOF = false;
        _stderrEOF = false;
        _terminationCallbackCalled = false;
        
        // Retain the wrapper using associated object to ensure it stays alive
        objc_setAssociatedObject(_task, @selector(handleTermination:), self, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return self;
}

- (bool)start {
    if (!_task || _isRunning) {
        if (_errorCallback) {
            NSString* errorMsg = @"Process already running or invalid task";
            _errorCallback([errorMsg UTF8String], _errorUserData);
        }
        return false;
    }
    
    @try {
        NSString* launchPath = [_task launchPath];
        if (!launchPath || [launchPath length] == 0) {
            if (_errorCallback) {
                _errorCallback([@"Launch path is empty" UTF8String], _errorUserData);
            }
            return false;
        }
        
        if ([launchPath hasPrefix:@"/"]) {
            NSFileManager* fileManager = [NSFileManager defaultManager];
            if (![fileManager fileExistsAtPath:launchPath]) {
                if (_errorCallback) {
                    NSString* errorMsg = [NSString stringWithFormat:@"Launch path does not exist: %@", launchPath];
                    _errorCallback([errorMsg UTF8String], _errorUserData);
                }
                return false;
            }
            
            if (![fileManager isExecutableFileAtPath:launchPath]) {
                if (_errorCallback) {
                    NSString* errorMsg = [NSString stringWithFormat:@"Launch path is not executable: %@", launchPath];
                    _errorCallback([errorMsg UTF8String], _errorUserData);
                }
                return false;
            }
        }
        
        [_task launch];
        _isRunning = true;
        
        [_stdoutHandle readInBackgroundAndNotify];
        [_stderrHandle readInBackgroundAndNotify];
        
        return true;
    } @catch (NSException* exception) {
        if (_errorCallback) {
            NSString* errorMsg = [NSString stringWithFormat:@"Failed to launch process: %@ - %@", 
                                  [exception name], [exception reason]];
            _errorCallback([errorMsg UTF8String], _errorUserData);
        }
        return false;
    }
}

- (void)setOnStdout:(ObsidianProcessOutputCallback)callback userData:(void*)userData {
    _stdoutCallback = callback;
    _stdoutUserData = userData;
}

- (void)setOnStderr:(ObsidianProcessOutputCallback)callback userData:(void*)userData {
    _stderrCallback = callback;
    _stderrUserData = userData;
}

- (void)setOnTermination:(ObsidianProcessTerminationCallback)callback userData:(void*)userData {
    _terminationCallback = callback;
    _terminationUserData = userData;
}

- (void)setOnError:(ObsidianProcessErrorCallback)callback userData:(void*)userData {
    _errorCallback = callback;
    _errorUserData = userData;
}

- (void)terminate {
    if (_task && _isRunning) {
        [_task terminate];
    }
}

- (bool)isRunning {
    return _isRunning && _task && [_task isRunning];
}

- (int)waitUntilExit {
    if (!_task || !_isRunning) {
        return -1;
    }
    
    [_task waitUntilExit];
    _isRunning = false;
    _exitCode = [_task terminationStatus];
    
    return _exitCode;
}

- (int)getExitCode {
    if (!_task || _isRunning) {
        return -1;
    }
    return _exitCode;
}

- (int32_t)getProcessIdentifier {
    if (!_task || !_isRunning) {
        return -1;
    }
    return [_task processIdentifier];
}

- (void)handleStdoutData:(NSNotification*)notification {
    NSData* data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
    NSFileHandle* fileHandle = [notification object];
    
    if (data && [data length] > 0) {
        NSString* outputString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        
        if (outputString && [outputString length] > 0) {
            const char* outputStr = [outputString UTF8String];
            if (outputStr && _stdoutCallback) {
                if ([NSThread isMainThread]) {
                    _stdoutCallback(outputStr, false, _stdoutUserData);
                } else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        if (self->_stdoutCallback) {
                            self->_stdoutCallback(outputStr, false, self->_stdoutUserData);
                        }
                    });
                }
            }
        }
        
        [fileHandle readInBackgroundAndNotify];
    } else {
        _stdoutEOF = true;
        [self checkAndCleanup];
    }
}

- (void)handleStderrData:(NSNotification*)notification {
    NSData* data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
    NSFileHandle* fileHandle = [notification object];
    
    if (data && [data length] > 0) {
        NSString* outputString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        
        if (outputString && [outputString length] > 0) {
            const char* outputStr = [outputString UTF8String];
            if (outputStr && _stderrCallback) {
                if ([NSThread isMainThread]) {
                    _stderrCallback(outputStr, true, _stderrUserData);
                } else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        if (self->_stderrCallback) {
                            self->_stderrCallback(outputStr, true, self->_stderrUserData);
                        }
                    });
                }
            }
        }
        
        [fileHandle readInBackgroundAndNotify];
    } else {
        _stderrEOF = true;
        [self checkAndCleanup];
    }
}

- (void)handleTermination:(NSNotification*)notification {
    _isRunning = false;
    _exitCode = [_task terminationStatus];
    
    if (_terminationCallback && !_terminationCallbackCalled) {
        _terminationCallbackCalled = true;
        _terminationCallback(_exitCode, _terminationUserData);
    }
    
    [self checkAndCleanup];
}

- (void)checkAndCleanup {
    if (!_isRunning && _stdoutEOF && _stderrEOF) {
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                         name:NSFileHandleReadCompletionNotification
                                                       object:_stdoutHandle];
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                         name:NSFileHandleReadCompletionNotification
                                                       object:_stderrHandle];
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                         name:NSTaskDidTerminateNotification
                                                       object:_task];
        
        if (_stdoutHandle) {
            [_stdoutHandle closeFile];
        }
        if (_stderrHandle) {
            [_stderrHandle closeFile];
        }
    }
}

- (void)dealloc {
    // Clean up observers
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    // Terminate task if still running
    if (_task && _isRunning) {
        [_task terminate];
    }
    
    // Mark as terminated to allow cleanup
    _isRunning = false;
    _stdoutEOF = true;
    _stderrEOF = true;
    
    // Close file handles
    if (_stdoutHandle) {
        [_stdoutHandle closeFile];
    }
    if (_stderrHandle) {
        [_stderrHandle closeFile];
    }
}

@end

// C interface implementation
extern "C" {

ObsidianProcessHandle obsidian_macos_create_process(ObsidianProcessParams params) {
    ObsidianProcessWrapper* wrapper = [[ObsidianProcessWrapper alloc] initWithParams:params];
    if (wrapper) {
        return (__bridge_retained void*)wrapper;
    }
    return nullptr;
}

bool obsidian_macos_process_start(ObsidianProcessHandle handle) {
    if (!handle) {
        return false;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    return [wrapper start];
}

void obsidian_macos_process_set_on_stdout(ObsidianProcessHandle handle,
                                          ObsidianProcessOutputCallback callback,
                                          void* userData) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    [wrapper setOnStdout:callback userData:userData];
}

void obsidian_macos_process_set_on_stderr(ObsidianProcessHandle handle,
                                         ObsidianProcessOutputCallback callback,
                                         void* userData) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    [wrapper setOnStderr:callback userData:userData];
}

void obsidian_macos_process_set_on_termination(ObsidianProcessHandle handle,
                                              ObsidianProcessTerminationCallback callback,
                                              void* userData) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    [wrapper setOnTermination:callback userData:userData];
}

void obsidian_macos_process_set_on_error(ObsidianProcessHandle handle,
                                         ObsidianProcessErrorCallback callback,
                                         void* userData) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    [wrapper setOnError:callback userData:userData];
}

void obsidian_macos_process_terminate(ObsidianProcessHandle handle) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    [wrapper terminate];
}

bool obsidian_macos_process_is_running(ObsidianProcessHandle handle) {
    if (!handle) {
        return false;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    return [wrapper isRunning];
}

int obsidian_macos_process_wait_until_exit(ObsidianProcessHandle handle) {
    if (!handle) {
        return -1;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    return [wrapper waitUntilExit];
}

int obsidian_macos_process_get_exit_code(ObsidianProcessHandle handle) {
    if (!handle) {
        return -1;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    return [wrapper getExitCode];
}

int32_t obsidian_macos_process_get_pid(ObsidianProcessHandle handle) {
    if (!handle) {
        return -1;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge ObsidianProcessWrapper*)handle;
    return [wrapper getProcessIdentifier];
}

bool obsidian_macos_process_is_valid(ObsidianProcessHandle handle) {
    return handle != nullptr;
}

void obsidian_macos_destroy_process(ObsidianProcessHandle handle) {
    if (!handle) {
        return;
    }
    
    ObsidianProcessWrapper* wrapper = (__bridge_transfer ObsidianProcessWrapper*)handle;
    // ARC will handle deallocation
    (void)wrapper; // Suppress unused variable warning
}

} // extern "C"

