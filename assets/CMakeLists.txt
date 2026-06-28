set(ASSETS
    # Imported from my dotfiles, but should probably draw something better. Like an alui logo or something
    # Doing logos for some of my projects that I plan to keep stronger guarantees for would probably be fun (and really
    # fucking hard)
    eepyfox.png

    # Guess who discovered the star brush in krita?
    # (drew both of these in like 15 mins combined with no real plan, I just needed something to work with for the image
    # layout tests)
    128x256.png
    256x128.png
)
foreach(ASSET IN LISTS ASSETS)
    message(STATUS "Copying ${ASSET}")
    configure_file(${ASSET} ${CMAKE_BINARY_DIR}/assets/${ASSET} COPYONLY)
endforeach()
