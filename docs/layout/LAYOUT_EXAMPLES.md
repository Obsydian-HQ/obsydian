# Layout System Usage Examples

**Version**: 1.0  
**Date**: December 2025  
**Platform**: macOS (primary implementation)

This document provides comprehensive examples of using the Obsidian layout system. For API reference, see [LAYOUT_API.md](LAYOUT_API.md).

## Table of Contents

1. [Basic Examples](#basic-examples)
2. [Advanced Examples](#advanced-examples)
3. [Common Patterns](#common-patterns)
4. [Real-World Examples](#real-world-examples)
5. [Best Practices](#best-practices)

## Basic Examples

### Example 1: Simple Vertical Stack

A basic vertical stack with three buttons:

```cpp
#include <obsidian/obsidian.h>
#include <iostream>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Simple VStack");
    
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::Center);
    vstack.setPadding(Padding::all(20.0));
    
    Button button1, button2, button3;
    button1.create("Button 1", 0, 0, 100, 30);
    button2.create("Button 2", 0, 0, 100, 30);
    button3.create("Button 3", 0, 0, 100, 30);
    
    vstack.addChild(button1);
    vstack.addChild(button2);
    vstack.addChild(button3);
    
    vstack.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Result**: Three buttons stacked vertically in the center of the window with 10px spacing and 20px padding on all sides.

---

### Example 2: Simple Horizontal Stack

A basic horizontal stack with three buttons:

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Simple HStack");
    
    HStack hstack;
    hstack.setSpacing(15.0);
    hstack.setAlignment(layout::Alignment::Middle);
    hstack.setPadding(Padding::all(20.0));
    
    Button button1, button2, button3;
    button1.create("Left", 0, 0, 80, 30);
    button2.create("Center", 0, 0, 80, 30);
    button3.create("Right", 0, 0, 80, 30);
    
    hstack.addChild(button1);
    hstack.addChild(button2);
    hstack.addChild(button3);
    
    hstack.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Result**: Three buttons arranged horizontally in the middle of the window with 15px spacing and 20px padding.

---

### Example 3: ZStack Overlay

Overlaying buttons with different sizes:

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "ZStack Overlay");
    
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::Center);
    
    Button button1, button2, button3;
    button1.create("Bottom Layer", 0, 0, 200, 100);
    button2.create("Middle Layer", 0, 0, 150, 70);
    button3.create("Top Layer", 0, 0, 120, 50);
    
    // Add in order: bottom to top
    zstack.addChild(button1);  // Bottom (largest)
    zstack.addChild(button2);  // Middle
    zstack.addChild(button3);  // Top (smallest, visible)
    
    zstack.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Result**: Three buttons overlaid at the center of the window. The top button (smallest) is visible, with the others behind it.

---

### Example 4: Using Spacers

Using spacers to create flexible layouts:

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Spacer Demo");
    
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::TopLeading);
    vstack.setPadding(Padding::all(20.0));
    
    Button topButton;
    topButton.create("Top Button", 0, 0, 150, 30);
    
    Spacer spacer;
    spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt
    
    Button bottomButton;
    bottomButton.create("Bottom Button", 0, 0, 150, 30);
    
    vstack.addChild(topButton);
    vstack.addChild(spacer);  // Expands to fill space
    vstack.addChild(bottomButton);
    
    vstack.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Result**: Top button at the top, bottom button at the bottom, with spacer expanding to fill the space between them.

---

## Advanced Examples

### Example 5: Nested Stacks

Combining VStack and HStack for complex layouts:

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Nested Stacks");
    
    // Outer VStack
    VStack outerVStack;
    outerVStack.setSpacing(20.0);
    outerVStack.setAlignment(layout::Alignment::TopLeading);
    outerVStack.setPadding(Padding::all(30.0));
    
    // Top HStack
    HStack topHStack;
    topHStack.setSpacing(15.0);
    topHStack.setAlignment(layout::Alignment::Middle);
    
    Button top1, top2, top3;
    top1.create("Top 1", 0, 0, 100, 30);
    top2.create("Top 2", 0, 0, 100, 30);
    top3.create("Top 3", 0, 0, 100, 30);
    
    topHStack.addChild(top1);
    topHStack.addChild(top2);
    topHStack.addChild(top3);
    
    // Middle Spacer
    Spacer middleSpacer;
    middleSpacer.create(0.0, 50.0, 0.0, 0.0);  // min height 50pt
    
    // Bottom HStack
    HStack bottomHStack;
    bottomHStack.setSpacing(15.0);
    bottomHStack.setAlignment(layout::Alignment::Middle);
    
    Button bottom1, bottom2, bottom3;
    bottom1.create("Bottom 1", 0, 0, 100, 30);
    bottom2.create("Bottom 2", 0, 0, 100, 30);
    bottom3.create("Bottom 3", 0, 0, 100, 30);
    
    bottomHStack.addChild(bottom1);
    bottomHStack.addChild(bottom2);
    bottomHStack.addChild(bottom3);
    
    // Add to outer VStack
    // Note: Stacks need to be added to window first to be valid
    // For nested stacks, we need to use a different approach
    // This example shows the concept - actual implementation may vary
    
    outerVStack.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Note**: Nested stacks require careful handling. This example demonstrates the concept. For actual nested stack implementation, see the layout showcase example.

---

### Example 6: Alignment Showcase

Demonstrating different alignment options:

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Alignment Showcase");
    
    // VStack with Leading alignment
    VStack vstackLeading;
    vstackLeading.setSpacing(10.0);
    vstackLeading.setAlignment(layout::Alignment::Leading);
    vstackLeading.setPadding(Padding::all(20.0));
    
    Button btn1, btn2, btn3;
    btn1.create("Leading 1", 0, 0, 120, 30);
    btn2.create("Leading 2", 0, 0, 120, 30);
    btn3.create("Leading 3", 0, 0, 120, 30);
    
    vstackLeading.addChild(btn1);
    vstackLeading.addChild(btn2);
    vstackLeading.addChild(btn3);
    
    vstackLeading.addToWindow(window);
    window.show();
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Variations**: Change `setAlignment()` to:
- `layout::Alignment::Center` - Centers buttons horizontally
- `layout::Alignment::Trailing` - Aligns buttons to the right
- `layout::Alignment::TopLeading` - Positions container at top-left
- `layout::Alignment::BottomCenter` - Positions container at bottom-center

---

### Example 7: Dynamic Layout Updates

Adding and removing children dynamically:

```cpp
#include <obsidian/obsidian.h>
#include <vector>

using namespace obsidian;

int main() {
    App app;
    app.initialize();
    
    Window window;
    window.create(800, 600, "Dynamic Layout");
    
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::Center);
    vstack.setPadding(Padding::all(20.0));
    
    // Create buttons
    std::vector<Button> buttons;
    for (int i = 0; i < 5; i++) {
        buttons.emplace_back();
        std::string title = "Button " + std::to_string(i + 1);
        buttons[i].create(title, 0, 0, 120, 30);
        vstack.addChild(buttons[i]);
    }
    
    vstack.addToWindow(window);
    window.show();
    
    // Layout automatically updates when children are added/removed
    // Example: Remove first button after 2 seconds (in real app, use timers/callbacks)
    // vstack.removeChild(buttons[0]);
    
    AppCallbacks callbacks;
    app.run(callbacks);
    
    return 0;
}
```

**Note**: Layout automatically updates when children are added or removed. No manual `updateLayout()` call is needed.

---

## Common Patterns

### Pattern 1: Form Layout

A typical form with labels and inputs (conceptual - uses buttons as placeholders):

```cpp
VStack form;
form.setSpacing(15.0);
form.setAlignment(layout::Alignment::TopLeading);
form.setPadding(Padding::symmetric(30.0, 40.0));

// Form fields (conceptual - actual implementation would use TextField)
Button nameLabel, nameInput;
Button emailLabel, emailInput;
Button submitButton;

nameLabel.create("Name:", 0, 0, 100, 25);
nameInput.create("", 0, 0, 300, 30);
emailLabel.create("Email:", 0, 0, 100, 25);
emailInput.create("", 0, 0, 300, 30);
submitButton.create("Submit", 0, 0, 100, 35);

form.addChild(nameLabel);
form.addChild(nameInput);
form.addChild(emailLabel);
form.addChild(emailInput);

Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);
form.addChild(spacer);

form.addChild(submitButton);
```

---

### Pattern 2: Toolbar Layout

A horizontal toolbar with buttons:

```cpp
HStack toolbar;
toolbar.setSpacing(10.0);
toolbar.setAlignment(layout::Alignment::Middle);
toolbar.setPadding(Padding::symmetric(10.0, 20.0));

Button save, open, newDoc;
save.create("Save", 0, 0, 80, 30);
open.create("Open", 0, 0, 80, 30);
newDoc.create("New", 0, 0, 80, 30);

toolbar.addChild(save);
toolbar.addChild(open);
toolbar.addChild(newDoc);

Spacer spacer;
spacer.create(50.0, 0.0, 0.0, 0.0);  // min width 50pt
toolbar.addChild(spacer);

Button help;
help.create("Help", 0, 0, 80, 30);
toolbar.addChild(help);
```

---

### Pattern 3: Card Layout

A card-like layout with header, content, and footer:

```cpp
VStack card;
card.setSpacing(10.0);
card.setAlignment(layout::Alignment::TopLeading);
card.setPadding(Padding::all(20.0));

// Header
Button header;
header.create("Card Header", 0, 0, 300, 40);

// Content
VStack content;
content.setSpacing(8.0);
content.setAlignment(layout::Alignment::Leading);

Button line1, line2, line3;
line1.create("Content line 1", 0, 0, 280, 25);
line2.create("Content line 2", 0, 0, 280, 25);
line3.create("Content line 3", 0, 0, 280, 25);

content.addChild(line1);
content.addChild(line2);
content.addChild(line3);

// Footer
HStack footer;
footer.setSpacing(10.0);
footer.setAlignment(layout::Alignment::Middle);

Button ok, cancel;
ok.create("OK", 0, 0, 80, 30);
cancel.create("Cancel", 0, 0, 80, 30);

footer.addChild(ok);
footer.addChild(cancel);

// Note: Nested stacks require special handling in current implementation
// This is a conceptual example
```

---

## Real-World Examples

### Example 8: Settings Panel

A settings panel layout (conceptual):

```cpp
VStack settingsPanel;
settingsPanel.setSpacing(20.0);
settingsPanel.setAlignment(layout::Alignment::TopLeading);
settingsPanel.setPadding(Padding::all(30.0));

// Title
Button title;
title.create("Settings", 0, 0, 200, 40);

// Section 1: Appearance
VStack appearanceSection;
appearanceSection.setSpacing(10.0);
appearanceSection.setAlignment(layout::Alignment::Leading);

Button appearanceTitle;
appearanceTitle.create("Appearance", 0, 0, 150, 30);

Button themeLabel, themeOption1, themeOption2;
themeLabel.create("Theme:", 0, 0, 100, 25);
themeOption1.create("Light", 0, 0, 100, 30);
themeOption2.create("Dark", 0, 0, 100, 30);

HStack themeOptions;
themeOptions.setSpacing(10.0);
themeOptions.addChild(themeOption1);
themeOptions.addChild(themeOption2);

// Section 2: Behavior
// Similar structure...

// Save/Cancel buttons at bottom
HStack actionButtons;
actionButtons.setSpacing(15.0);
actionButtons.setAlignment(layout::Alignment::Trailing);

Button save, cancel;
save.create("Save", 0, 0, 100, 35);
cancel.create("Cancel", 0, 0, 100, 35);

actionButtons.addChild(cancel);
actionButtons.addChild(save);

// Combine sections (conceptual - nested stacks need special handling)
```

---

### Example 9: Dashboard Layout

A dashboard with multiple sections (conceptual):

```cpp
VStack dashboard;
dashboard.setSpacing(25.0);
dashboard.setAlignment(layout::Alignment::TopLeading);
dashboard.setPadding(Padding::all(30.0));

// Header
HStack header;
header.setSpacing(15.0);
header.setAlignment(layout::Alignment::Middle);

Button title;
title.create("Dashboard", 0, 0, 200, 40);

Spacer headerSpacer;
headerSpacer.create(100.0, 0.0, 0.0, 0.0);

Button refresh;
refresh.create("Refresh", 0, 0, 100, 35);

header.addChild(title);
header.addChild(headerSpacer);
header.addChild(refresh);

// Stats Row
HStack statsRow;
statsRow.setSpacing(20.0);
statsRow.setAlignment(layout::Alignment::Top);

Button stat1, stat2, stat3, stat4;
stat1.create("Stat 1: 100", 0, 0, 150, 80);
stat2.create("Stat 2: 200", 0, 0, 150, 80);
stat3.create("Stat 3: 300", 0, 0, 150, 80);
stat4.create("Stat 4: 400", 0, 0, 150, 80);

statsRow.addChild(stat1);
statsRow.addChild(stat2);
statsRow.addChild(stat3);
statsRow.addChild(stat4);

// Content area with spacer
Spacer contentSpacer;
contentSpacer.create(0.0, 50.0, 0.0, 0.0);

// Footer
HStack footer;
footer.setSpacing(10.0);
footer.setAlignment(layout::Alignment::Center);

Button footerText;
footerText.create("Status: Ready", 0, 0, 200, 25);

footer.addChild(footerText);
```

---

## Best Practices

### 1. Set Configuration Before Adding Children

**Good**:
```cpp
VStack vstack;
vstack.setSpacing(10.0);  // Set before adding children
vstack.setAlignment(layout::Alignment::Center);
vstack.setPadding(Padding::all(20.0));

Button btn1, btn2, btn3;
// ... create buttons ...
vstack.addChild(btn1);
vstack.addChild(btn2);
vstack.addChild(btn3);
```

**Avoid**:
```cpp
VStack vstack;
vstack.addChild(btn1);  // Adding before configuration
vstack.setSpacing(10.0);  // Causes unnecessary layout update
```

---

### 2. Use Appropriate Spacing Values

**Good**:
```cpp
vstack.setSpacing(10.0);  // Consistent spacing
hstack.setSpacing(15.0);  // Slightly larger for horizontal
```

**Avoid**:
```cpp
vstack.setSpacing(0.1);  // Too small, hard to see
vstack.setSpacing(500.0);  // Too large, wastes space
```

---

### 3. Choose Meaningful Alignments

**Good**:
```cpp
// For vertical lists, align left (Leading)
vstack.setAlignment(layout::Alignment::Leading);

// For centered content
vstack.setAlignment(layout::Alignment::Center);

// For toolbars, align middle vertically
hstack.setAlignment(layout::Alignment::Middle);
```

**Avoid**:
```cpp
// Don't use complex alignments when simple ones work
vstack.setAlignment(layout::Alignment::TopLeading);  // Use Leading instead
```

---

### 4. Use Spacers for Flexible Layouts

**Good**:
```cpp
VStack vstack;
Button top, bottom;
Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt

vstack.addChild(top);
vstack.addChild(spacer);  // Expands to fill space
vstack.addChild(bottom);
```

**Avoid**:
```cpp
// Don't use multiple spacers when one works
vstack.addChild(spacer1);
vstack.addChild(spacer2);  // Usually unnecessary
```

---

### 5. Keep Nesting Reasonable

**Good**:
```cpp
VStack outer;
HStack inner;
// 2 levels of nesting - reasonable
```

**Avoid**:
```cpp
VStack level1;
HStack level2;
VStack level3;
HStack level4;
VStack level5;
// 5+ levels - may impact performance
```

---

### 6. Handle Window Resize

**Good**: Window resize is handled automatically by NSLayoutConstraint. Your layout will automatically adapt.

**No action needed**: The layout system handles window resize automatically.

---

### 7. Use Padding for Visual Spacing

**Good**:
```cpp
VStack vstack;
vstack.setPadding(Padding::all(20.0));  // Consistent padding
// or
vstack.setPadding(Padding::symmetric(15.0, 30.0));  // Different vertical/horizontal
```

**Avoid**:
```cpp
// Don't use excessive padding
vstack.setPadding(Padding::all(200.0));  // Too much
```

---

## Troubleshooting

### Layout Not Updating

**Problem**: Changes to spacing/alignment/padding don't take effect.

**Solution**: Ensure the stack is valid and added to a window. Changes take effect immediately if the stack is already in a window hierarchy.

### Spacer Not Expanding

**Problem**: Spacer doesn't fill available space.

**Solution**: 
1. Ensure the spacer is between other elements (not at the edges)
2. Check that the parent stack is properly sized (added to window)
3. Verify min/max constraints aren't preventing expansion

### Alignment Not Working

**Problem**: Alignment doesn't position children as expected.

**Solution**:
1. For VStack: Alignment controls horizontal positioning (Leading/Center/Trailing)
2. For HStack: Alignment controls vertical positioning (Top/Middle/Bottom)
3. Combined alignments (e.g., TopLeading) affect container positioning, not child positioning

### Window Resize Not Working

**Problem**: Layout doesn't adapt when window is resized.

**Solution**: This should work automatically. If it doesn't:
1. Ensure the stack is properly added to the window
2. Verify constraints are active
3. Check that the window is resizable

---

**See Also**:
- [Layout API Reference](LAYOUT_API.md) - Complete API documentation
- [Layout System Plan](LAYOUT_SYSTEM_PLAN.md) - Implementation details
- [Layout Checkpoints](LAYOUT_SYSTEM_CHECKPOINTS.md) - Development progress
