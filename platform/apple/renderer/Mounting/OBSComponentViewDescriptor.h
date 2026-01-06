/**
 * OBSComponentViewDescriptor
 *
 * Holds a native view instance and metadata.
 * The __strong qualifier ensures ARC properly retains the view.
 *
 * Directly inspired by React Native's RCTComponentViewDescriptor.
 */

#import <AppKit/AppKit.h>
#import "OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Descriptor for a component view.
 *
 * Contains the native view instance and metadata for bookkeeping.
 * The view is STRONGLY RETAINED by this descriptor.
 */
struct OBSComponentViewDescriptor {
    /**
     * The native view instance.
     * STRONG reference - ARC will retain this view.
     */
    __strong NSView<OBSComponentViewProtocol> * view;
    
    /**
     * Whether this view should be recycled when unmounted.
     * Some views cannot be recycled (e.g., custom user views).
     */
    bool shouldBeRecycled;
    
    // Default constructor
    OBSComponentViewDescriptor()
        : view(nil)
        , shouldBeRecycled(true)
    {}
    
    // Constructor with view
    explicit OBSComponentViewDescriptor(NSView<OBSComponentViewProtocol> *v, bool recycle = true)
        : view(v)
        , shouldBeRecycled(recycle)
    {}
    
    // Copy constructor - ARC handles retain
    OBSComponentViewDescriptor(const OBSComponentViewDescriptor &other)
        : view(other.view)
        , shouldBeRecycled(other.shouldBeRecycled)
    {}
    
    // Move constructor
    OBSComponentViewDescriptor(OBSComponentViewDescriptor &&other) noexcept
        : view(other.view)
        , shouldBeRecycled(other.shouldBeRecycled)
    {
        other.view = nil;
    }
    
    // Copy assignment
    OBSComponentViewDescriptor &operator=(const OBSComponentViewDescriptor &other) {
        if (this != &other) {
            view = other.view;
            shouldBeRecycled = other.shouldBeRecycled;
        }
        return *this;
    }
    
    // Move assignment
    OBSComponentViewDescriptor &operator=(OBSComponentViewDescriptor &&other) noexcept {
        if (this != &other) {
            view = other.view;
            shouldBeRecycled = other.shouldBeRecycled;
            other.view = nil;
        }
        return *this;
    }
};

// Equality comparison
inline bool operator==(const OBSComponentViewDescriptor &lhs, const OBSComponentViewDescriptor &rhs) {
    return lhs.view == rhs.view;
}

inline bool operator!=(const OBSComponentViewDescriptor &lhs, const OBSComponentViewDescriptor &rhs) {
    return lhs.view != rhs.view;
}

NS_ASSUME_NONNULL_END
