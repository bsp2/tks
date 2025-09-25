// ----
// ----    file: TKUI_CommonDialogs.h
// ----  author: (c) 2005-2025 Bastian Spiegel <bs@tkscript.de>
// ----    date: 13-Jun-2005, 16-Jun-2005
// ---- license: Distributed under terms of the Lesser General Public License (LGPL).
// ----          See <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
// ----    info: Helper functions to access common native OS dialog windows.
// ----
#ifndef TKUI_COMMON_DIALOGS_H__
#define TKUI_COMMON_DIALOGS_H__

YG("ui");

/* @function TKUI_OpenFileDialog,String suggestedPath,String suggestedFile,String windowTitle,StringArray filterNames,StringArray filterStrings,boolean bMultipleSelection,boolean bDirectoriesOnly:Object
 */

// ----
// ---- YAC_Object *_suggestedPath, // initial directory
// ---- YAC_Object *_suggestedFile, // name of last selected file
// ---- YAC_Object *_windowTitle,   // caption of the (modal) dialog window
// ---- YAC_Object *_filterNames,   // array of filter names. each array index must have a corresponding element in the _filterStrings array.
// ---- YAC_Object *_filterStrings, // array of filter strings. Each String may contain a ';' separated list of file filters, e.g. "*.h;*.cpp;*.cc".
// ---- sSI         _bMultipleSelection, // 0=single selection, 1=multiple selection
// ---- sSI         _bDirectoriesOnly,   // 0=also allow file selections, 1=only select directories
// ---- YAC_Value *_r               // returns the selected file name (String) resp. the selected file names (StringArray), depending on _bMultipleSelection.
// ----
YF void YAC_CALL TKUI_OpenFileDialog(YAC_Object *_suggestedPath, YAC_Object *_suggestedFile, YAC_Object *_windowTitle, YAC_Object *_filterNames, YAC_Object *_filterStrings, sSI _bMultipleSelection, sSI _bDirectoriesOnly, YAC_Value *_r);


/* @function TKUI_SaveFileDialog,String suggestedPath,String suggestedFile,String windowTitle,StringArray filterNames,StringArray filterStrings,boolean bDirectoriesOnly:String
 */

// ----
// ----
// ---- YAC_Object *_suggestedPath, // initial directory
// ---- YAC_Object *_suggestedFile, // suggested name for saved file
// ---- YAC_Object *_windowTitle,   // caption of the (modal) dialog window
// ---- YAC_Object *_filterNames,   // array of filter names. each array index must have a corresponding element in the _filterStrings array.
// ---- YAC_Object *_filterStrings, // array of filter strings. Each String may contain a ';' separated list of file filters, e.g. "*.h;*.cpp;*.cc".
// ---- sSI         _bDirectoriesOnly,   // 0=save to a file, 1=save to a directory/create directory
// ---- YAC_Value *_r               // returns the selected file name (String) or null indicating that the user cancelled the dialog
// ----
YF void YAC_CALL TKUI_SaveFileDialog(YAC_Object *_suggestedPath, YAC_Object *_suggestedFile, YAC_Object *_windowTitle, YAC_Object *_filterNames, YAC_Object *_filterStrings, sSI _bDirectoriesOnly, YAC_Value *_r);


#endif // TKUI_COMMON_DIALOGS_H__
