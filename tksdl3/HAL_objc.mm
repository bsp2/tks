#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <string>
#include <vector>
#include <stdio.h>

void hal_macos_window_to_front(void/*NSWindow*/ *_window) {
   NSWindow *window = (NSWindow*)_window;
   // NSLog(@"%@", NSStringFromClass([window class]));
   // printf("hello, world.\n");
   // // [window setLevel:kCGDesktopWindowLevel + 1] ;
   [window makeKeyAndOrderFront:nil];  // crashes when _window does not belong to current process. does not work, anyway.
   [NSApp activateIgnoringOtherApps:YES];  // this works

#if 0
   // ----- enumerate windows. only works with windows that belong to the current process.
   // ----- this is just to validate the syswm.info.cocoa.window handle (=> it's correct)
   // https://developer.apple.com/library/archive/samplecode/SonOfGrab/Listings/Controller_m.html#//apple_ref/doc/uid/DTS10004490-Controller_m-DontLinkElementID_4
   // https://github.com/trevordevore/lc-macos-toolset/blob/master/NSWindow/nswindow.lcb
   // https://itecnote.com/tecnote/objective-c-getting-window-number-through-osx-accessibility-api/
   CGWindowListOption listOptions = kCGWindowListOptionAll;
   // returns array of dictionaries
   // Here's an exemplary dictionary returned:
   // {
   //     kCGWindowAlpha = 1;
   //     kCGWindowBounds =         {
   //         Height = 442;
   //         Width = 475;
   //         X = 3123;
   //         Y = "-118";
   //     };
   //     kCGWindowIsOnscreen = 1;
   //     kCGWindowLayer = 0;
   //     kCGWindowMemoryUsage = 907184;
   //     kCGWindowName = Untitled;
   //     kCGWindowNumber = 7328;
   //     kCGWindowOwnerName = TextEdit;
   //     kCGWindowOwnerPID = 20706;
   //     kCGWindowSharingState = 1;
   //     kCGWindowStoreType = 2;
   //     kCGWindowWorkspace = 3;
   // },
   CFArrayRef windowList = CGWindowListCopyWindowInfo(listOptions, kCGNullWindowID);
   NSEnumerator *e = [windowList objectEnumerator];
   id object;
   while (object = [e nextObject])
   {
      // do something with object
      // NSLog(@"%@", NSStringFromClass([object class]));
      // => 2023-02-23 20:45:51.591 tks.bin[24330:2396210] __NSDictionaryI    (I==immutable)
      NSDictionary *dict = object;
      NSData *data = [dict objectForKey:@"kCGWindowNumber"];
      // NSLog(@"%@", NSStringFromClass([data class]));
      // => 2023-02-23 20:49:11.729 tks.bin[24360:2398697] __NSCFNumber
      NSNumber *windowNumber = (NSNumber*)data;
      // printf("xxx kCGWindowNumber=%ld\n", (long)[windowNumber integerValue]);
      NSWindow *w = [NSApp windowWithWindowNumber: [windowNumber integerValue]];
      union {
         void *any;
         unsigned long u64;
      } m;
      m.any = w;
      if(w) {
         printf("xxx kCGWindowNumber=%ld  w=%llu\n", (long)[windowNumber integerValue], m.u64);
         [w makeKeyAndOrderFront:nil];  // no crash (self to front)
      }
   }
   // for(CFDictionaryRef dict in windowList)
   // {
   //    printf("xxx dict count=%u\n", CFDictionaryCount(dict));
   // }
   // NSWindow *w = [NSApp windowWithWindowNumber: windowNumber]
   CFRelease(windowList);
#endif // 0

}
