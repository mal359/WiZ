#ifndef TOOLBAR_H
#define TOOLBAR_H

#define IDM_TOOLBAR         500 // ID for the toolbar.
#define IDB_BMP             700 // Bitmap ID for the toolbar.

// Global variable for Toolbar window
extern HWND hWndToolbar;

// Function protoptype for Creating Toolbar
BOOL CreateTBar(HWND);
void UpdateToolbar(VOID);

// Function prototype for toolbar button maintenance
void UpdateButton(UINT iID, UINT iFlags);

#endif //TOOLBAR_H

