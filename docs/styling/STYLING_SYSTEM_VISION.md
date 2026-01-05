# Obsidian Styling System - Vision & Architecture

## Executive Summary

This document outlines a comprehensive vision for implementing a modern, cross-platform styling system for the Obsidian framework. The system will provide consistent styling capabilities across macOS, iOS, Android, Linux, Windows, and Web platforms while respecting platform-specific conventions and maintaining performance.

## Philosophy & Goals

### Core Principles

1. **Design Tokens First**: All styling values originate from design tokens - platform-agnostic semantic values that can be transformed for each platform
2. **Declarative & Composable**: Inspired by SwiftUI's modifier system and Tailwind's utility-first approach, allowing chained styling modifiers
3. **Platform-Aware**: Respects native platform conventions while maintaining cross-platform consistency
4. **Performance-Centric**: Zero-cost abstractions where possible, efficient style resolution, minimal allocations
5. **Extensible**: Easy to add new styling properties, components, and platforms
6. **Developer-Friendly**: Clean, intuitive APIs that feel natural to C++ developers

### What We're Solving

**Problems with Existing Systems:**
- **SwiftUI**: Excellent developer experience but Apple-only, difficult to extend
- **Flutter**: Great theming but Dart-specific, can be verbose
- **Tailwind**: Utility-first is powerful but requires build-time compilation, CSS-specific
- **Qt Stylesheets**: String-based, error-prone, limited type safety
- **Native APIs**: Platform-specific, inconsistent across platforms, verbose

**Our Approach:**
- Combine best ideas: SwiftUI's modifier pattern + Tailwind's utility-first + Flutter's theming
- Type-safe C++ API with compile-time optimizations
- Runtime theming support with design token resolution
- Platform-specific optimizations while maintaining API consistency

## Architecture Overview

### Three-Layer Architecture

```
┌─────────────────────────────────────────────┐
│   Public C++ API (Declarative Styling)      │
│   - Style modifiers, themes, tokens          │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│   Core Styling Engine (Platform-Agnostic)   │
│   - Token resolution, style computation      │
│   - Theme management, style inheritance      │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│   Platform FFI Layer (Platform-Specific)    │
│   - Native style application                 │
│   - Platform token transformations           │
└─────────────────────────────────────────────┘
```

## Design Tokens System

### Token Hierarchy

Design tokens are the foundation. Following W3C Design Tokens Community Group standards:

```
Tokens
├── Colors
│   ├── Semantic (primary, secondary, error, success)
│   ├── Semantic Variants (primary.50 to primary.900)
│   ├── Neutral (background, foreground, border)
│   └── Platform Adaptations (iOS/macOS dynamic colors, Android material)
├── Typography
│   ├── Font Families (system, mono, serif)
│   ├── Font Sizes (xs, sm, base, lg, xl, 2xl, ...)
│   ├── Font Weights (thin, light, normal, medium, semibold, bold, black)
│   └── Line Heights (tight, normal, relaxed, loose)
├── Spacing
│   ├── Scale (0, 1, 2, 4, 8, 12, 16, 24, 32, 48, 64, ...)
│   └── Semantic (xs, sm, md, lg, xl, 2xl)
├── Borders
│   ├── Width (0, 1, 2, 4, 8)
│   ├── Radius (none, sm, md, lg, xl, full)
│   └── Styles (solid, dashed, dotted)
├── Shadows
│   ├── Elevation Levels (0-24)
│   └── Platform-Specific (iOS blur, Android elevation, Web CSS)
├── Materials (Platform-Specific)
│   ├── macOS (NSVisualEffectView materials)
│   ├── iOS (UIVisualEffectView blur styles)
│   └── Other (fallback to solid colors)
└── Motion/Animation
    ├── Duration (fast: 150ms, normal: 300ms, slow: 500ms)
    └── Easing (linear, ease-in, ease-out, ease-in-out)
```

### Token Resolution

Tokens resolve through multiple layers:
1. **Default Theme** (Obsidian defaults)
2. **User Theme** (app-provided theme)
3. **Platform Adaptations** (macOS/iOS dynamic colors, Android Material)
4. **Runtime Overrides** (component-specific overrides)

## Public API Design

### Declarative Style Modifiers (SwiftUI-Inspired)

```cpp
namespace obsidian::style {

// Style modifiers are composable and chainable
Button button;
button.create("Click Me", 0, 0, 100, 40)
    .style()
    .backgroundColor(Color::semantic("primary"))
    .foregroundColor(Color::semantic("on-primary"))
    .font(Font::body().weight(FontWeight::semibold))
    .padding(Padding::all(16))
    .borderRadius(BorderRadius::md())
    .shadow(Shadow::elevation(2));
```

### Theme System (Flutter-Inspired)

```cpp
// Define a theme
Theme myTheme = Theme::create()
    .colors(ColorScheme::dark()
        .primary(Color::rgb(0x3B82F6))
        .background(Color::rgb(0x1F2937))
        .surface(Color::rgb(0x374151)))
    .typography(Typography::system()
        .body(Font::size(16).weight(FontWeight::regular))
        .heading(Font::size(24).weight(FontWeight::bold)))
    .spacing(SpacingScale::default());

// Apply to app/window
App app;
app.setTheme(myTheme);
```

### Utility Classes (Tailwind-Inspired)

```cpp
// Quick utility modifiers
button.style()
    .p(16)           // padding: 16
    .px(24)          // padding-x: 24
    .py(12)          // padding-y: 12
    .bgBlue500()     // background: blue-500
    .textWhite()     // text: white
    .roundedLg()     // border-radius: large
    .shadowMd();     // shadow: medium
```

### Design Token Access

```cpp
// Direct token access
Color primary = Color::token("color.primary");
Color surface = Color::token("color.surface");
double spacingMd = Spacing::token("spacing.md");
Font bodyFont = Font::token("typography.body");
```

## Project Structure

### Directory Layout

```
obsidian/
├── core/
│   └── styling/                    # Core styling engine (platform-agnostic)
│       ├── BUILD
│       ├── design_tokens.h/cpp     # Design token definitions and resolution
│       ├── theme.h/cpp             # Theme system
│       ├── style.h/cpp             # Style computation engine
│       ├── color.h/cpp             # Color system
│       ├── typography.h/cpp        # Typography system
│       ├── spacing.h/cpp           # Spacing system
│       └── style_resolver.h/cpp    # Resolves styles with inheritance
├── include/
│   └── obsidian/
│       └── style/                  # Public styling API
│           ├── style.h             # Main styling API
│           ├── theme.h             # Theme API
│           ├── color.h             # Color API
│           ├── font.h              # Font/Typography API
│           ├── modifier.h          # Style modifier base
│           └── tokens.h            # Design token access
├── packages/
│   ├── apple/
│   │   └── styling/                # Apple platform styling
│   │       ├── macos_style_ffi.h/mm
│   │       └── ios_style_ffi.h/mm
│   ├── android/
│   │   └── styling/                # Android platform styling
│   │       └── android_style_ffi.h/cpp
│   ├── linux/
│   │   └── styling/                # Linux (GTK) styling
│   │       └── gtk_style_ffi.h/cpp
│   ├── windows/
│   │   └── styling/                # Windows styling
│   │       └── win32_style_ffi.h/cpp
│   └── web/
│       └── styling/                # Web (WASM) styling
│           └── web_style_ffi.h/cpp
└── docs/
    └── styling/
        ├── STYLING_SYSTEM_VISION.md         # This document
        ├── STYLING_API_REFERENCE.md         # API documentation
        ├── STYLING_EXAMPLES.md              # Usage examples
        ├── DESIGN_TOKENS.md                 # Design token reference
        └── PLATFORM_ADAPTATIONS.md          # Platform-specific notes
```

## Component Integration

### How Styling Touches Every Component

**1. Base Style Interface**
All components inherit or compose a `Stylable` interface:

```cpp
namespace obsidian::style {

class Stylable {
public:
    Style& style();
    const Style& style() const;
    void applyStyle(const Style& style);
    void setTheme(const Theme& theme);
};

} // namespace obsidian::style
```

**2. Component-Specific Styles**
Each component has a `StyleModifier<T>` specialization:

```cpp
// Button-specific style modifiers
class ButtonStyleModifier : public StyleModifier<Button> {
public:
    ButtonStyleModifier& backgroundColor(const Color& color);
    ButtonStyleModifier& foregroundColor(const Color& color);
    ButtonStyleModifier& font(const Font& font);
    ButtonStyleModifier& padding(const Padding& padding);
    ButtonStyleModifier& borderRadius(double radius);
    // ... more button-specific styles
};
```

**3. Platform-Specific Application**
Platform FFI layers translate styles to native APIs:

- **macOS/iOS**: NSButton attributes, NSColor, NSFont, CALayer properties
- **Android**: Material Theme attributes, ColorStateList, TypedArray
- **Linux**: GTK CSS, GtkStyleContext
- **Windows**: GDI+ brushes, Direct2D, WinUI styles
- **Web**: CSS custom properties, inline styles, CSS classes

## API Usage Examples

### Example 1: Basic Button Styling

```cpp
#include <obsidian/obsidian.h>
#include <obsidian/style/style.h>

using namespace obsidian;
using namespace obsidian::style;

Button button;
button.create("Submit", 0, 0, 120, 40)
    .style()
    .backgroundColor(Color::rgb(0x3B82F6))    // Blue
    .foregroundColor(Color::rgb(0xFFFFFF))    // White
    .font(Font::body().weight(FontWeight::semibold))
    .padding(Padding::symmetric(12, 24))
    .borderRadius(8)
    .shadow(Shadow::elevation(2));
```

### Example 2: Theme-Based Styling

```cpp
// Define theme
Theme appTheme = Theme::create()
    .colorScheme(ColorScheme::light()
        .primary(Color::rgb(0x3B82F6))
        .secondary(Color::rgb(0x10B981))
        .error(Color::rgb(0xEF4444))
        .background(Color::rgb(0xFFFFFF))
        .surface(Color::rgb(0xF9FAFB))
        .onPrimary(Color::rgb(0xFFFFFF))
        .onSurface(Color::rgb(0x111827)))
    .typography(Typography::system()
        .body(Font::size(16).lineHeight(1.5))
        .heading(Font::size(24).weight(FontWeight::bold)));

// Apply to app
App app;
app.setTheme(appTheme);

// Components automatically use theme
Button primaryButton;
primaryButton.create("Primary Action", 0, 0, 150, 44)
    .style()
    .variant(ButtonVariant::primary);  // Uses theme.primary

Button secondaryButton;
secondaryButton.create("Secondary Action", 0, 0, 150, 44)
    .style()
    .variant(ButtonVariant::secondary);  // Uses theme.secondary
```

### Example 3: Utility-First Approach

```cpp
// Quick styling with utility methods
VStack vstack;
vstack.style()
    .p(24)              // padding: 24
    .gap(16)            // gap: 16
    .bgGray100();       // background: gray-100

Button btn1, btn2, btn3;
btn1.create("Button 1", 0, 0, 100, 40)
    .style().p(12).bgBlue500().textWhite().roundedMd();
    
btn2.create("Button 2", 0, 0, 100, 40)
    .style().p(12).bgGreen500().textWhite().roundedMd();
    
btn3.create("Button 3", 0, 0, 100, 40)
    .style().p(12).bgRed500().textWhite().roundedMd();
```

### Example 4: Platform-Specific Adaptations

```cpp
// System automatically adapts to platform
Button button;
button.create("Native Button", 0, 0, 120, 44)
    .style()
    .useNativeStyle(true)  // Use platform-native button style
    .font(Font::system())  // Uses system font
    .tintColor(Color::systemBlue());  // iOS/macOS system blue
```

### Example 5: Dark Mode Support

```cpp
// Automatic dark mode support
Theme theme = Theme::create()
    .colorScheme(ColorScheme::adaptive()
        .light(ColorScheme::light()
            .background(Color::rgb(0xFFFFFF))
            .foreground(Color::rgb(0x000000)))
        .dark(ColorScheme::dark()
            .background(Color::rgb(0x1F2937))
            .foreground(Color::rgb(0xFFFFFF))));

App app;
app.setTheme(theme);
// Automatically switches based on system preference
```

### Example 6: Material Design (Android)

```cpp
// Material Design specific
#ifdef __ANDROID__
Button button;
button.create("Material Button", 0, 0, 120, 48)
    .style()
    .materialElevation(4)           // Material shadow/elevation
    .rippleEffect(true)             // Material ripple
    .cornerRadius(4);               // Material corner radius
#endif
```

### Example 7: Responsive Design

```cpp
// Style adapts to window size
Window window;
window.create(800, 600, "Responsive App");

VStack container;
container.style()
    .padding(Padding::responsive()
        .small(Padding::all(8))
        .medium(Padding::all(16))
        .large(Padding::all(24)))
    .gap(Spacing::responsive()
        .small(8)
        .medium(16)
        .large(24));
```

## Platform-Specific Considerations

### macOS/iOS

**Native Integration:**
- Dynamic colors (NSColor dynamic colors, UIColor dynamic colors)
- SF Symbols integration for icons
- Visual Effect Materials (NSVisualEffectView, UIVisualEffectView)
- Accessibility (VoiceOver, Dynamic Type)
- Dark Mode automatic adaptation

**Implementation:**
- NSButton styling via attributed strings, bezel styles
- CALayer properties for shadows, borders, corner radius
- NSColor/UIColor semantic colors
- NSFont/UIFont dynamic type

### Android

**Material Design:**
- Material Theme integration
- Material 3 design tokens
- ColorStateList for state-based colors
- TypedArray for style attributes
- Elevation system for shadows

**Implementation:**
- MaterialButton, MaterialCardView styling
- Theme attributes via Context.obtainStyledAttributes
- AndroidX Material Components theming
- Ripple effects, touch feedback

### Linux (GTK)

**GTK Integration:**
- GTK CSS styling
- GTK Theme support (Adwaita, etc.)
- GTK StyleContext for computed styles
- CSS Provider for custom styles

**Implementation:**
- GtkStyleContext API
- CSS classes for component styling
- GTK theme color variables
- Custom CSS provider injection

### Windows

**WinUI/Native:**
- Fluent Design System
- WinUI 3 styling
- Acrylic/blur materials
- System color themes

**Implementation:**
- GDI+ for basic styling
- Direct2D for advanced rendering
- WinUI controls styling
- Windows.UI.Xaml.Media brushes

### Web (WASM)

**Web Technologies:**
- CSS Custom Properties (CSS Variables)
- CSS Grid/Flexbox for layout
- CSS-in-JS or inline styles
- Tailwind CSS class generation (optional)

**Implementation:**
- DOM manipulation for style application
- CSS custom properties for theming
- CSS classes for utility-first approach
- Web Components shadow DOM styling

## Testing Strategy

### Unit Tests

**Token Resolution Tests:**
- Token lookup and resolution
- Theme inheritance
- Platform-specific token transformations
- Token value computation

**Style Computation Tests:**
- Style modifier chaining
- Style merging and inheritance
- Style override resolution
- Style validation

### Integration Tests

**Component Styling Tests:**
- Each component type with various styles
- Theme application across components
- Style updates at runtime
- Style persistence

**Platform Tests:**
- Native style rendering verification
- Platform-specific style adaptations
- Performance benchmarks
- Memory leak detection

### Visual Regression Tests

**GUI Verification Apps:**
Create dedicated test applications for each platform:

```
examples/
└── styling_showcase/
    ├── BUILD
    ├── main.cpp
    ├── color_showcase.cpp      # Color system demo
    ├── typography_showcase.cpp  # Typography demo
    ├── theme_showcase.cpp       # Theme switching demo
    ├── component_showcase.cpp   # All components styled
    └── responsive_showcase.cpp  # Responsive design demo
```

**Visual Test Suites:**
- Screenshot comparison tests
- Automated visual validation
- Cross-platform visual consistency checks

## Implementation Phases

### Phase 1: Foundation (Months 1-2)

**Design Tokens System:**
- Token data structures
- Token resolution engine
- Default token values
- Platform token transformations

**Core Style Engine:**
- Style data structures
- Style computation
- Style inheritance
- Style caching

**Public API Skeleton:**
- Basic StyleModifier interface
- Color, Font, Padding types
- Theme structure

### Phase 2: Basic Styling (Months 3-4)

**Platform Implementation (macOS first):**
- macOS FFI styling layer
- NSColor/NSFont integration
- Basic style application to Button, TextView
- Visual verification

**Component Integration:**
- Button styling
- TextView styling
- Basic container styling (VStack, HStack)

**Theme System:**
- Theme definition
- Theme application
- Theme switching

### Phase 3: Advanced Features (Months 5-6)

**Remaining Components:**
- All component types styled
- Layout container styling
- List and Table styling

**Advanced Features:**
- Dark mode support
- Responsive design
- Animation/motion system
- Material/shadow system

**Platform Expansion:**
- iOS implementation
- Android implementation
- Linux (GTK) implementation

### Phase 4: Polish & Optimization (Months 7-8)

**Performance:**
- Style caching optimization
- Memory usage optimization
- Rendering performance

**Developer Experience:**
- Documentation
- Examples
- Error messages
- Debugging tools

**Cross-Platform:**
- Windows implementation
- Web (WASM) implementation
- Cross-platform consistency validation

## Integration with Existing Codebase

### Component Updates

**Existing Components Need:**
1. `Stylable` interface implementation
2. Style storage in Impl classes
3. Style application in platform FFI layers
4. Default style definitions

**Example: Button Integration**

```cpp
// include/obsidian/button.h additions
class Button {
    // ... existing methods ...
    
    /**
     * Get style modifier for this button
     */
    style::ButtonStyleModifier style();
};

// src/obsidian/button.cpp additions
#include "obsidian/style/style.h"

style::ButtonStyleModifier Button::style() {
    return style::ButtonStyleModifier(*this);
}
```

### Layout System Integration

The existing layout system (constraints) works independently but can be enhanced:
- Style-driven layout constraints (padding, margins)
- Responsive constraints based on window size
- Theme-aware spacing

### Window Integration

Windows become theme containers:
```cpp
Window window;
window.setTheme(theme);  // Applies theme to all child components
```

## Design Decisions

### Why Design Tokens?

- **Consistency**: Single source of truth for all design values
- **Theming**: Easy theme creation and switching
- **Platform Adaptation**: Transform tokens per platform
- **Maintainability**: Change tokens, update everywhere

### Why Declarative Modifiers?

- **Composability**: Chain multiple modifiers
- **Readability**: Self-documenting code
- **Type Safety**: Compile-time checks
- **Familiar**: SwiftUI developers will feel at home

### Why Utility Classes?

- **Speed**: Quick prototyping and common cases
- **Consistency**: Predefined scales prevent arbitrary values
- **Expressiveness**: Combine declarative with utility

### Why Not CSS/String-Based?

- **Type Safety**: Catch errors at compile-time
- **Performance**: No string parsing
- **Tooling**: Better IDE support, autocomplete
- **Integration**: Works with C++ ecosystem

## Performance Considerations

### Style Resolution

- **Caching**: Cache resolved styles per component
- **Lazy Evaluation**: Resolve styles only when needed
- **Immutable Styles**: Styles are immutable, can be shared
- **Style Pools**: Reuse common style objects

### Platform Integration

- **Native APIs**: Use platform-native styling APIs directly
- **Minimal Allocations**: Prefer stack allocation where possible
- **Efficient Updates**: Batch style updates, minimize redraws

## Future Enhancements

### Advanced Features

1. **Animation System**: Smooth transitions between styles
2. **State-Based Styling**: Hover, focus, active, disabled states
3. **Custom Properties**: User-defined design tokens
4. **Style Variants**: Predefined style combinations
5. **Style Inspector**: Runtime style debugging tools
6. **Hot Reload**: Theme/style hot reload in development

### Design Tool Integration

1. **Figma Plugin**: Export design tokens from Figma
2. **Design Token Sync**: Keep tokens in sync with design files
3. **Visual Editor**: GUI tool for creating themes

### Advanced Theming

1. **Component-Level Themes**: Override theme per component tree
2. **Dynamic Themes**: Generate themes at runtime
3. **Theme Variants**: Multiple theme variants per app
4. **Theme Transitions**: Smooth theme switching animations

## Success Metrics

1. **Developer Experience**: 
   - Time to style a component < 30 seconds
   - Zero styling-related compile errors in examples
   - 100% API documentation coverage

2. **Performance**:
   - Style resolution < 1ms per component
   - Theme switch < 100ms for entire window
   - Zero memory leaks in style system

3. **Consistency**:
   - Visual consistency across platforms > 95%
   - Design token coverage > 90% of style properties
   - Cross-platform test coverage > 80%

4. **Adoption**:
   - All example apps use styling system
   - Styling API used in all new components
   - Backward compatibility maintained

## Research References

### Design Token Standards
- W3C Design Tokens Community Group
- Style Dictionary (Amazon)
- Design Tokens Format (DTCG)

### Framework Inspirations
- **SwiftUI**: ViewModifier pattern, Environment values, declarative syntax
- **Flutter**: ThemeData, Theme widgets, Material Design tokens
- **Tailwind CSS**: Utility-first, design token scales, responsive design
- **Qt**: QStyle system, property-based styling
- **Material Design 3**: Token system, adaptive colors, typography scale

### Cross-Platform Patterns
- **React Native**: StyleSheet API, platform-specific styles
- **Xamarin.Forms**: Resource dictionaries, styles, themes
- **.NET MAUI**: Resource system, style inheritance

## Next Steps

1. **Review & Refinement**: Review this vision with team, refine based on feedback
2. **Prototype**: Build minimal prototype (design tokens + Button styling on macOS)
3. **Validate**: Test prototype with real use cases
4. **Iterate**: Refine based on prototype learnings
5. **Implement**: Begin Phase 1 implementation

---

**Document Version**: 1.0  
**Last Updated**: December 2025  
**Status**: Vision Document - Ready for Review
