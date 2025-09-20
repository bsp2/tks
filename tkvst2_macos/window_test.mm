
// https://www.oreilly.com/library/view/building-cocoa-applications/0596002351/ch04.html (not working)
// https://eezytutorials.com/ios/objective-c/objective-c-delegation-by-example.php
// https://stackoverflow.com/questions/626898/how-do-i-create-delegates-in-objective-c

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <stdio.h>
#include <yac.h>


@interface DemoView : NSView <NSWindowDelegate>
{
}
- (void)drawRect:(NSRect)rect;  // instance method interface

// -(void)windowWillClose:(NSNotification *)notification;
// -(void)windowDidMove:(NSNotification *)notification;

@end


@implementation DemoView 

- (void)drawRect:(NSRect)rect   // instance method implementation
{
   printf("xxx DemoView::drawRect\n");
   [[NSColor whiteColor] set];   // set the drawing color to white
   NSRectFill([self bounds]);    // fill the view with white
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

-(BOOL)windowShouldClose:(NSWindow *)win
{
   printf("xxx DemoDelegate::windowShouldClose\n");
   // return NO;
   return YES;
}

-(void)windowWillClose:(NSNotification *)notification
{
   printf("xxx DemoDelegate::windowWillClose\n");
}

-(void)windowDidMove:(NSNotification *)notification
{
   printf("xxx DemoDelegate::windowDidMove\n");
}

- (void) keyDown: (NSEvent*) event {
	if ([event isARepeat] == NO) {
		NSLog(@"Key down: %d", [event keyCode]);
	}
}

@end  // end of DemoView implementation


// // @interface DemoWindowController : NSWindowController<NSWindowDelegate>
// // @end 

// // @implementation DemoWindowController 
// // @end 

YF void YAC_CALL tkvst2_window_test (void) {

   NSWindow *myWindow;      // typed pointer to NSWindow object
   NSView   *myView;
   NSWindowController *myDelegate;
   NSRect    graphicsRect;

   // initialize the rectangle variable
   graphicsRect = NSMakeRect(100.0, 350.0, 400.0, 400.0);

   myWindow = [ [NSWindow alloc]
                 initWithContentRect: graphicsRect
                         //styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask
                           styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                             backing:NSBackingStoreBuffered
                               defer:NO
                ];

   [myWindow setTitle:@"Tiny Application Window"];

   // create and initialize the DemoView instance
   myView = [[[DemoView alloc] initWithFrame:graphicsRect] autorelease];
 
   [myWindow setContentView:myView ];    // set window's view

   // // NSWindowController *windowController = [[NSWindowController alloc] initWithWindow:myWindow];
   // // [windowController autorelease];

   // // myWindow.delegate = (NSWindowDelegate*)myWindow;
   // // myWindow.delegate = windowController;
   // // myWindow.delegate = myWindow;

    // // myDelegate = [[DemoWindowController alloc] initWithWindow:myWindow];
    // // [myDelegate autorelease];

   [myWindow setDelegate:(id<NSWindowDelegate>)myView ];
    // [myWindow setDelegate:myDelegate ];

   [myWindow makeKeyAndOrderFront: nil];

}

