
// based on <https://warwick.ac.uk/fac/sci/systemsbiology/staff/dyer/software/cpposxfileopendialog/>
//  (note) https://serialcoder.dev/text-tutorials/macos-tutorials/save-and-open-panels-in-macos-apps/
//  (note) https://developer.apple.com/documentation/appkit/nsopenpanel
//  (note) https://developer.apple.com/documentation/appkit/nssavepanel

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <string>
#include <vector>

std::vector<std::string>openFileDialog(
   char const * const aTitle,
   char const * const aDefaultDirName,
   char const * const aDefaultFileName,
   const std::vector<std::string> & filters,
   bool bMultipleSelection,
   bool bDirectoriesOnly,
   bool bSave
                                       ) {

   int i;
   std::vector<std::string> fileList;
   // Create a File Open Dialog class.
   union {
      NSSavePanel *any;
      NSSavePanel *save;
      NSOpenPanel *open;
   } dlg;
   dlg.any = bSave ? [NSSavePanel savePanel] : [NSOpenPanel openPanel];
   [dlg.any setLevel:CGShieldingWindowLevel()];
   // Set array of file types

   NSMutableArray * fileTypesArray = NULL;
   if(filters.size() > 0)
   {
      fileTypesArray = [NSMutableArray array];
      for (i = 0;i < filters.size(); i++)
      {
         NSString * filt =[NSString stringWithUTF8String:filters[i].c_str()];
         [fileTypesArray addObject:filt];
      }
   }

   // Enable options in the dialog.
   if(!bSave)
   {
      if(bDirectoriesOnly)
      {
         [dlg.open setCanChooseFiles:NO];
         [dlg.open setCanChooseDirectories:YES];
      }
      else
         [dlg.open setCanChooseFiles:YES];

      if(bMultipleSelection)
         [dlg.open setAllowsMultipleSelection:TRUE];
   }

   [dlg.any setCanCreateDirectories:YES];
   // [dlg setShowsTagsField:YES];

   // (note) deprecated in macOS 11+
   if(NULL != fileTypesArray)
      [dlg.any setAllowedFileTypes:fileTypesArray];

   if(NULL != aDefaultDirName)
   {
      printf("[trc] TKUI_CommonDialogs_macos_obj: aDefaultDirName=\"%s\"\n", aDefaultDirName);
      // [dlg.any setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String:aDefaultPathName ] ] ];
      [dlg.any setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:aDefaultDirName] ] ]; // stringByExpandingTildeInPath
   }

#if 1
   if(NULL != aDefaultFileName)
   {
      printf("[trc] TKUI_CommonDialogs_macos_obj: aDefaultDirName=\"%s\" aDefaultFileName=\"%s\"\n", aDefaultDirName, aDefaultFileName);
      [dlg.any setNameFieldStringValue:[NSString stringWithUTF8String:aDefaultFileName] ];
   }
#endif

   // Display the dialog box. If the OK pressed,
   // process the files.
   if ( [dlg.any runModal] == NSOKButton ) {

      if(bSave)
      {
         NSURL *url = [dlg.save URL];
         fileList.push_back(std::string([[url path] UTF8String]));
      }
      else
      {
         // Gets list of all files selected
         NSArray *files = [dlg.open URLs];
         // Loop through the files and process them.
         for( i = 0; i < [files count]; i++ ) {
            // Do something with the filename.
            fileList.push_back(std::string([[[files objectAtIndex:i] path] UTF8String]));
         }
      }
   }
   return fileList;
}
