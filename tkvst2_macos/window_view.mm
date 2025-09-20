/// window_view.mm
///
/// (c) 2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 25Feb2023
/// changed: 08Mar2023, 24Oct2023, 26Oct2023
///
///
///

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <stdio.h>
#include <yac.h>
#include "tkvst2.h"
#include "VST2Plugin.h"

extern void RemoveVST2WindowByNSView (void *_nsView);

class TKVST2Window;
typedef struct tkvst2_window_view_s {
   VST2Plugin   *plugin;
   TKVST2Window *tkvst2window;
   NSWindow     *nsWindow;
   NSView       *nsView;
} tkvst2_window_view_t;


#define TKVST2_MAX_WINDOWTITLE_SIZE 256
class TKVST2Window {
public:
   TKVST2Window *next;
   VST2Plugin   *plugin;
   tkvst2_window_view_t window_view;
   char          window_title[TKVST2_MAX_WINDOWTITLE_SIZE];
   sBool         b_allow_redirect_close_to_hide;
};


@interface VST2View : NSView <NSWindowDelegate> {
   @public
   VST2Plugin *plugin;
   TKVST2Window *tkvst2window;
}
- (void)drawRect:(NSRect)rect;
@end


@implementation VST2View 

- (void)drawRect:(NSRect)rect {
   // printf("xxx VST2View::drawRect\n");
   [[NSColor whiteColor] set];
   NSRectFill([self bounds]);
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

-(BOOL)windowShouldClose:(NSWindow *)win
{
   printf("[trc] VST2View::windowShouldClose\n");
   // return NO;
   return YES;
}

-(void)windowWillClose:(NSNotification *)notification
{
   printf("[trc] VST2View::windowWillClose\n");
   RemoveVST2WindowByNSView(self);
}

-(void)windowDidMove:(NSNotification *)notification
{
   NSRect frame = [ tkvst2window->window_view.nsWindow frame ];
   plugin->ui_window_x = (int)frame.origin.x;
   plugin->ui_window_y = (int)frame.origin.y;
   printf("[trc] VST2View::windowDidMove: p=(%d; %d)\n", plugin->ui_window_x, plugin->ui_window_y);
}

- (void) keyDown: (NSEvent*) event {
   // (note) does not report RCTRL
	if ([event isARepeat] == NO) {
		NSLog(@"[trc] VST2View::keyDown: %d", [event keyCode]);
      plugin->callOnMacOSKeyDown([event keyCode]);
	}
}

- (void) flagsChanged: (NSEvent*) event {
   // (note) modifier keys
   NSLog(@"[trc] VST2View::flagsChanged: %d", [event keyCode]);
   plugin->callOnMacOSKeyDown([event keyCode]);
}

@end  // VST2View


void tkvst2_macos_create_window_view(tkvst2_window_view_t *_winView, int _w, int _h, const char *_title) {
   NSWindow *myWindow;
   VST2View *myView;
   NSWindowController *myDelegate;
   NSRect    graphicsRect;

   //// graphicsRect = NSMakeRect(-1920, 1000, _w, _h);
   // graphicsRect = NSMakeRect(0, 0, _w, _h);
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

   // [myWindow setTitle:@"VST2 Window"];
   [myWindow setTitle:[NSString stringWithUTF8String:_title] ];


   myView = [[[VST2View alloc] initWithFrame:graphicsRect] autorelease];
   myView->plugin       = _winView->plugin;
   myView->tkvst2window = _winView->tkvst2window;
 
   [myWindow setContentView:myView];
   [myWindow setDelegate:(id<NSWindowDelegate>)myView ];
   [myWindow makeKeyAndOrderFront: nil];

   _winView->nsWindow = myWindow;
   _winView->nsView   = myView;
}

void tkvst2_macos_get_window_geometry(tkvst2_window_view_t *_winView, int *x, int *y, int *w, int *h) {
   // NSRect frame = [ [ _winView->nsWindow contentView ] frame ];
   NSRect win = [_winView->nsWindow frame];
   *x = (int)win.origin.x;
   *y = (int)win.origin.y;

   NSRect content = [[_winView->nsWindow contentView] frame];
   *w = (int)content.size.width;
   *h = (int)content.size.height;
}

void tkvst2_macos_set_window_geometry(tkvst2_window_view_t *_winView, int x, int y, int w, int h) {
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

void tkvst2_macos_close_window_view(tkvst2_window_view_t *_winView) {
   [_winView->nsWindow close];
}

void tkvst2_macos_setvisible_window_view(tkvst2_window_view_t *_winView, int bHide) {
   [_winView->nsWindow setIsVisible:bHide?YES:NO];
}

int tkvst2_macos_isvisible_window_view(tkvst2_window_view_t *_winView) {
   BOOL bVisible = [_winView->nsWindow isVisible];
   return bVisible ? 1 : 0;
}

void tkvst2_macos_tofront_window_view(tkvst2_window_view_t *_winView) {
   [_winView->nsWindow makeKeyAndOrderFront: nil];
}

void tkvst2_macos_update_window_view(tkvst2_window_view_t *_winView) {
   [_winView->nsView setNeedsDisplay: true];
}
