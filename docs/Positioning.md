# Positioning

\brief This document describes how the library handles positioning

## Absolute positioning of root-level layouts only

Elements are positioned relatively, and the positioning is decided by the containing layout. These mechanics vary from layout to layout - see the docs for the respective layouts for information. It's important to note that in most cases, all the layouts are flex layouts internally, with only some exceptions. However, this is just an implementation detail.

Layouts themselves can be positioned absolutely, but only if the layout is a top-level layout, directly within the root-level GUI. These mechanics are meant to more closely resemble HTML, while still respecting that  game GUIs are far more complex in terms of positioning of individual elements.

If you need a specific layout system, you can create your own custom layout that respects absolute positions in components. This will not be a part of the library, however, as there's usually better ways to go about it.
