#import <Cocoa/Cocoa.h>
#include <gtk/gtk.h>
#include "dirbrowser.h"

/* The handler function type from dirbrowser.h */
typedef void (*XMMSDirHandler)(gchar *);

@interface XMMSDirBrowserDelegate : NSObject
@property (assign) XMMSDirHandler handler;
@end

@implementation XMMSDirBrowserDelegate
- (void)panelSelectionDidChange:(id)sender {
}
@end

void open_native_dir_browser(const char *title, const char *current_path, int mode, XMMSDirHandler handler) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setTitle:[NSString stringWithUTF8String:title]];
    [panel setCanChooseDirectories:YES];
    [panel setCanChooseFiles:YES];
    [panel setAllowsMultipleSelection:(mode == GTK_SELECTION_MULTIPLE)];
    
    if (current_path && strlen(current_path) > 0) {
        [panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:current_path]]];
    }
    
    if ([panel runModal] == NSModalResponseOK) {
        for (NSURL *url in [panel URLs]) {
            handler((gchar *)[[url path] UTF8String]);
        }
    }
    
    [pool release];
}
