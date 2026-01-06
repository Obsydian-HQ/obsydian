// Placeholder Kotlin file for Obsidian Android platform bindings
// This file ensures the kt_jvm_library target can build.
// Replace with actual Kotlin bindings when implementing Android platform features.

package obsidian.android

// Placeholder class to allow the target to build
class Placeholder {
    companion object {
        // JNI native library will be loaded at runtime via System.loadLibrary()
        // The native library (android_jni) is linked at the Android application level
        init {
            // System.loadLibrary("obsidian_android_jni")
        }
    }
}

