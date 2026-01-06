/**
 * Apple Platform Bindings - Swift Interface
 * 
 * Provides Swift interface for macOS platform features
 * This file replaces the Placeholder.swift file
 */

import Foundation
import AppKit
@_exported import ObsidianMacOS

/**
 * Swift wrapper for macOS window creation
 */
public class ObsidianWindow {
    private var handle: UnsafeMutableRawPointer?
    
    public init(width: Int, height: Int, title: String) {
        self.handle = title.withCString { cString in
            let params = ObsidianWindowParams(
                width: Int32(width),
                height: Int32(height),
                title: cString
            )
            return obsidian_macos_create_window(params)
        }
    }
    
    public func show() {
        if let handle = handle {
            obsidian_macos_show_window(handle)
        }
    }
    
    public func close() {
        if let handle = handle {
            obsidian_macos_destroy_window(handle)
            self.handle = nil
        }
    }
    
    public var isValid: Bool {
        guard let handle = handle else { return false }
        return obsidian_macos_window_is_valid(handle)
    }
    
    deinit {
        close()
    }
}

/**
 * Swift wrapper for macOS platform initialization
 */
public class ObsidianPlatform {
    public static func initialize() -> Bool {
        return obsidian_macos_platform_init()
    }
    
    public static func run() {
        obsidian_macos_platform_run()
    }
    
    public static func stop() {
        obsidian_macos_platform_stop()
    }
    
    public static func cleanup() {
        obsidian_macos_platform_cleanup()
    }
}

