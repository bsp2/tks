/// window_view.mm
///
/// (c) 2023-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 25Feb2023
/// changed: 08Mar2023, 24Oct2023, 26Oct2023, 01Jul2024, 03Jul2024, 05Jul2024
///
///
///

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <stdio.h>
#include <yac.h>
#define TKCLAP_WINDOW_SKIP_NS_WINDOW_VIEW_TYPES defined
#include "tkclap.h"
#include "CLAPPlugin.h"

extern void tkclap_window_remove_by_nsview (void *_nsView);

// class TKCLAPWindow;
// typedef struct tkclap_window_view_s {
//    CLAPPlugin   *plugin;
//    TKCLAPWindow *tkclapwindow;
//    NSWindow     *nsWindow;
//    NSView       *nsView;
// } tkclap_window_view_t;


// #define TKCLAP_MAX_WINDOWTITLE_SIZE 256
// class TKCLAPWindow {
// public:
//    TKCLAPWindow *next;
//    CLAPPlugin   *plugin;
//    tkclap_window_view_t window_view;
//    char          window_title[TKCLAP_MAX_WINDOWTITLE_SIZE];
//    sBool         b_allow_redirect_close_to_hide;
// };


@interface CLAPView : NSView <NSWindowDelegate> {
   @public
   CLAPPlugin   *plugin;
   TKCLAPWindow *tkclapwindow;
}
- (void)drawRect:(NSRect)rect;
@end


@implementation CLAPView 

- (void)drawRect:(NSRect)rect {
   // printf("xxx CLAPView::drawRect\n");
   [[NSColor whiteColor] set];
   NSRectFill([self bounds]);
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

-(BOOL)windowShouldClose:(NSWindow *)win
{
   printf("[trc] CLAPView::windowShouldClose\n");
   // return NO;
   return YES;
}

-(void)windowWillClose:(NSNotification *)notification
{
   printf("[trc] CLAPView::windowWillClose\n");
   tkclap_window_remove_by_nsview(self);
}

-(void)windowDidMove:(NSNotification *)notification
{
   NSRect frame = [ tkclapwindow->window_view.nsWindow frame ];
   plugin->ui_window_x = (int)frame.origin.x;
   plugin->ui_window_y = (int)frame.origin.y;
   printf("[trc] CLAPView::windowDidMove: p=(%d; %d)\n", plugin->ui_window_x, plugin->ui_window_y);
}

- (void) keyDown: (NSEvent*) event {
   // (note) does not report RCTRL
	if ([event isARepeat] == NO) {
		NSLog(@"[trc] CLAPView::keyDown: %d", [event keyCode]);
      plugin->callOnMacOSKeyDown([event keyCode]);
	}
}

- (void) flagsChanged: (NSEvent*) event {
   // (note) modifier keys
   NSLog(@"[trc] CLAPView::flagsChanged: %d", [event keyCode]);
   plugin->callOnMacOSKeyDown([event keyCode]);
}

@end  // CLAPView


void tkclap_macos_create_window_view(tkclap_window_view_t *_winView, int _w, int _h, const char *_title) {
   NSWindow *myWindow;
   CLAPView *myView;
   NSWindowController *myDelegate;
   NSRect    graphicsRect;

   // //// graphicsRect = NSMakeRect(-1920, 1000, _w, _h);
   // // graphicsRect = NSMakeRect(0, 0, _w, _h);
   NSPoint pos; 
   pos.x = ([[NSScreen mainScreen] frame].size.width  - _w) / 2;
   pos.y = ([[NSScreen mainScreen] frame].size.height - _h) / 2;
   graphicsRect = NSMakeRect(pos.x, pos.y, _w, _h);
   NSLog(@"[trc] tksvst2_macos_create_window_view: w=%d h=%d", _w, _h);

   myWindow = [ [NSWindow alloc]
                 initWithContentRect:graphicsRect
                           styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                             backing:NSBackingStoreBuffered
                               defer:NO
                ];

   // [myWindow setTitle:@"CLAP Window"];
   [myWindow setTitle:[NSString stringWithUTF8String:_title] ];


   myView = [[[CLAPView alloc] initWithFrame:graphicsRect] autorelease];
   myView->plugin       = _winView->plugin;
   myView->tkclapwindow = _winView->tkclapwindow;
 
   [myWindow setContentView:myView];
   [myWindow setDelegate:(id<NSWindowDelegate>)myView ];
   [myWindow makeKeyAndOrderFront: nil];

   _winView->nsWindow = myWindow;
   _winView->nsView   = myView;
}

void tkclap_macos_get_window_geometry(tkclap_window_view_t *_winView, int *x, int *y, int *w, int *h) {
   // // NSRect frame = [ [ _winView->nsWindow contentView ] frame ];
   NSRect win = [_winView->nsWindow frame];
   *x = (int)win.origin.x;
   *y = (int)win.origin.y;

   NSRect content = [[_winView->nsWindow contentView] frame];
   *w = (int)content.size.width;
   *h = (int)content.size.height;
}

void tkclap_macos_set_window_geometry(tkclap_window_view_t *_winView, int x, int y, int w, int h) {
#if 0
   {
      NSRect frame = [_winView->nsView frame];
      frame.origin.x = 0;
      frame.origin.y = 0;
      frame.size.width = w;
      frame.size.height = h;
      [_winView->nsView setFrame: frame ];
   }

   {
      NSRect frame = [_winView->nsWindow frame];
      frame.origin.x = x;
      frame.origin.y = y;
      frame.size.width = w;
      frame.size.height = h;
      [_winView->nsWindow setFrame: frame display: YES animate: YES];
   }
#else
   {
      NSRect frame = [_winView->nsWindow frame];
      frame.origin.x = x;
      frame.origin.y = y;
      frame.size.width = w;
      frame.size.height = h;
      [_winView->nsWindow setContentSize: frame.size ];
   }

   {
      NSRect frame = [_winView->nsWindow frame];
      frame.origin.x = x;
      frame.origin.y = y;
      // // frame.size.width = w;
      // // frame.size.height = h;
      [_winView->nsWindow setFrame: frame display: YES animate: YES];
   }
#endif

}

void tkclap_macos_close_window_view(tkclap_window_view_t *_winView) {
   [_winView->nsWindow close];
}

void tkclap_macos_setvisible_window_view(tkclap_window_view_t *_winView, int bHide) {
   [_winView->nsWindow setIsVisible:bHide?YES:NO];
}

int tkclap_macos_isvisible_window_view(tkclap_window_view_t *_winView) {
   BOOL bVisible = [_winView->nsWindow isVisible];
   return bVisible ? 1 : 0;
}

void tkclap_macos_tofront_window_view(tkclap_window_view_t *_winView) {
   [_winView->nsWindow makeKeyAndOrderFront: nil];
}

void tkclap_macos_update_window_view(tkclap_window_view_t *_winView) {
   [_winView->nsView setNeedsDisplay: true];
}

void *tkclap_macos_nswindow_get_nsview(void *nsWindow) {
   return (void*) [(NSWindow*)nsWindow contentView];
}
