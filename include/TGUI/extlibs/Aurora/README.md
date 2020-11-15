This folder contains the [Aurora](https://github.com/Bromeon/Aurora/) headers (downloaded in March 2016) of which some are used inside TGUI.

Only 3 changes were made to the project:
- The includes have been changed to look for the files inside the TGUI folder to not cause problems when another Aurora version would already be installed.
- The parameters of the PtrOwner constructor were renamed to avoid warnings with -Wshadow.
- Added override keyword to clone and getPointer functions in PtrOwner, CompactOwner and PtrIndirection structs to avoid warnings with -Wsuggest-override.
