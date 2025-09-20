/// VST2Plugin.cpp
///
/// (c) 2010-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 02Oct2010
/// changed: 03Oct2010, 04Oct2010, 05Oct2010, 09Oct2010, 10Oct2010, 13Oct2010, 15Oct2010
///          16Oct2010, 20Oct2010, 02Jul2011, 09Jan2012, 09May2013, 12May2013, 13May2013
///          08Mar2014, 19Jan2018, 20Jan2018, 24Jan2018, 25Jan2018, 26Jan2018, 27Jan2018
///          31Jan2018, 12Feb2018, 14Feb2018, 15Feb2018, 23Feb2018, 27Feb2018, 28Feb2018
///          02Mar2018, 03Apr2018, 22Jun2018, 29Jun2018, 06Jul2018, 19Aug2018, 07Jul2019
///          17Jul2019, 30Jul2019, 05Aug2019, 26Aug2019, 20May2021, 31Dec2021, 08Jan2022
///          11Jan2022, 27Feb2022, 01Jul2024
///
///
///

/// when defined, print wndprocedure debug messages
// #define WM_DEBUG defined

// (note) added while trying to debug crash in Waldorf Nave effEditOpen() (=> no use, REMOVE)
// (note) breaks Softube Tape UI
//#define SET_PFD_SUPPORT_OPENGL defined

// (note) added while trying to debug crash in Waldorf Nave effEditOpen() (=> no use, REMOVE)
// #define EARLY_WINDOW_CREATE defined

#include "tkvst2.h"

#include "VST2PinProperties.h"
#include "VST2ParameterProperties.h"

#include "VST2Plugin.h"
#include "../tkmidipipe/HostMIDIEvents.h"
#include "../tkmidipipe/ModInputFilter.h"

#include <math.h>

// (note) resume() >> startProcess() >> processReplacing() 
// (note) processReplacing() >> stopProcess() >> suspend() 


// If defined, check whether plugin tries to write beyond specified string boundaries (some buggy plugins do)
#define PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW defined

// If defined, check if processReplacing() produced any output
#define PARANOIA_CHECK_REPLACING_OUTPUT defined

#define DATABRIDGE_HOST_EVENT_NAME   "vst_databridge_ev"   // raised by any consumer, databridge-only host can wait for it
#define effDataBridgeGetNumChunksAvail  0x44894489

HANDLE VST2Plugin::databridge_host_ev = 0;

sUI VST2Plugin::static_shell_plugin_uid = 0u;
CRITICAL_SECTION VST2Plugin::mtx_shell_plugin_uid;

YAC_FunctionHandle tkvst2_audio_master_signal_funs[VST2PLUGIN_NUM_SIGNALS];

YAC_ContextHandle tkvst2_audio_master_script_context;

static sBool b_hide_vst_window_when_closed = YAC_TRUE;

#ifdef WM_DEBUG
#define Dwmprintf if(0);else yac_host->printf
#else
#define Dwmprintf if(1);else yac_host->printf
#endif // WM_DEBUG

#define Dopprintf if(1);else yac_host->printf


static CRITICAL_SECTION mtx_windows;
static sBool b_mtx_windows = 0;

static void LazyInitMtxWindows(void) {
   if(!b_mtx_windows)
   {
      ::InitializeCriticalSection(&mtx_windows);
      b_mtx_windows = 1;
   }
}

static inline void LockWindows(void) {
   ::EnterCriticalSection(&mtx_windows);
}
static inline void UnlockWindows(void) {
   ::LeaveCriticalSection(&mtx_windows);
}


#if 0
static CRITICAL_SECTION mtx_paramedit;
static sBool b_mtx_paramedit = 0;

static void LazyInitMtxParamEdit(void) {
   if(!b_mtx_paramedit)
   {
      ::InitializeCriticalSection(&mtx_paramedit);
      b_mtx_paramedit = 1;
   }
}

static inline void LockParamEdit(void) {
   ::EnterCriticalSection(&mtx_paramedit);
}
static inline void UnlockParamEdit(void) {
   ::LeaveCriticalSection(&mtx_paramedit);
}
#endif


// HANDLE ev_threadstart = NULL;



// (todo) use SetWindowLongPtr to get TKVST2Window* instead of the list lookup below


#ifdef WM_DEBUG
#define MAX_WMMSG	(sizeof(loc_wmtab)/sizeof(loc_wmtab[0]))

// wmtab borrowed from SDL Wmmsg.h
static char *loc_wmtab[] = {
	"WM_NULL",
	"WM_CREATE",
	"WM_DESTROY",
	"WM_MOVE",
	"UNKNOWN (4)",
	"WM_SIZE",
	"WM_ACTIVATE",
	"WM_SETFOCUS",
	"WM_KILLFOCUS",
	"UNKNOWN (9)",
	"WM_ENABLE",
	"WM_SETREDRAW",
	"WM_SETTEXT",
	"WM_GETTEXT",
	"WM_GETTEXTLENGTH",
	"WM_PAINT",
	"WM_CLOSE",
	"WM_QUERYENDSESSION",
	"WM_QUIT",
	"WM_QUERYOPEN",
	"WM_ERASEBKGND",
	"WM_SYSCOLORCHANGE",
	"WM_ENDSESSION",
	"UNKNOWN (23)",
	"WM_SHOWWINDOW",
	"UNKNOWN (25)",
	"WM_SETTINGCHANGE",
	"WM_DEVMODECHANGE",
	"WM_ACTIVATEAPP",
	"WM_FONTCHANGE",
	"WM_TIMECHANGE",
	"WM_CANCELMODE",
	"WM_SETCURSOR",
	"WM_MOUSEACTIVATE",
	"WM_CHILDACTIVATE",
	"WM_QUEUESYNC",
	"WM_GETMINMAXINFO",
	"UNKNOWN (37)",
	"WM_PAINTICON",
	"WM_ICONERASEBKGND",
	"WM_NEXTDLGCTL",
	"UNKNOWN (41)",
	"WM_SPOOLERSTATUS",
	"WM_DRAWITEM",
	"WM_MEASUREITEM",
	"WM_DELETEITEM",
	"WM_VKEYTOITEM",
	"WM_CHARTOITEM",
	"WM_SETFONT",
	"WM_GETFONT",
	"WM_SETHOTKEY",
	"WM_GETHOTKEY",
	"UNKNOWN (52)",
	"UNKNOWN (53)",
	"UNKNOWN (54)",
	"WM_QUERYDRAGICON",
	"UNKNOWN (56)",
	"WM_COMPAREITEM",
	"UNKNOWN (58)",
	"UNKNOWN (59)",
	"UNKNOWN (60)",
	"WM_GETOBJECT",
	"UNKNOWN (62)",
	"UNKNOWN (63)",
	"UNKNOWN (64)",
	"WM_COMPACTING",
	"UNKNOWN (66)",
	"UNKNOWN (67)",
	"WM_COMMNOTIFY",
	"UNKNOWN (69)",
	"WM_WINDOWPOSCHANGING",
	"WM_WINDOWPOSCHANGED",
	"WM_POWER",
	"UNKNOWN (73)",
	"WM_COPYDATA",
	"WM_CANCELJOURNAL",
	"UNKNOWN (76)",
	"UNKNOWN (77)",
	"WM_NOTIFY",
	"UNKNOWN (79)",
	"WM_INPUTLANGCHANGEREQUEST",
	"WM_INPUTLANGCHANGE",
	"WM_TCARD",
	"WM_HELP",
	"WM_USERCHANGED",
	"WM_NOTIFYFORMAT",
	"UNKNOWN (86)",
	"UNKNOWN (87)",
	"UNKNOWN (88)",
	"UNKNOWN (89)",
	"UNKNOWN (90)",
	"UNKNOWN (91)",
	"UNKNOWN (92)",
	"UNKNOWN (93)",
	"UNKNOWN (94)",
	"UNKNOWN (95)",
	"UNKNOWN (96)",
	"UNKNOWN (97)",
	"UNKNOWN (98)",
	"UNKNOWN (99)",
	"UNKNOWN (100)",
	"UNKNOWN (101)",
	"UNKNOWN (102)",
	"UNKNOWN (103)",
	"UNKNOWN (104)",
	"UNKNOWN (105)",
	"UNKNOWN (106)",
	"UNKNOWN (107)",
	"UNKNOWN (108)",
	"UNKNOWN (109)",
	"UNKNOWN (110)",
	"UNKNOWN (111)",
	"UNKNOWN (112)",
	"UNKNOWN (113)",
	"UNKNOWN (114)",
	"UNKNOWN (115)",
	"UNKNOWN (116)",
	"UNKNOWN (117)",
	"UNKNOWN (118)",
	"UNKNOWN (119)",
	"UNKNOWN (120)",
	"UNKNOWN (121)",
	"UNKNOWN (122)",
	"WM_CONTEXTMENU",
	"WM_STYLECHANGING",
	"WM_STYLECHANGED",
	"WM_DISPLAYCHANGE",
	"WM_GETICON",
	"WM_SETICON",
	"WM_NCCREATE",
	"WM_NCDESTROY",
	"WM_NCCALCSIZE",
	"WM_NCHITTEST",
	"WM_NCPAINT",
	"WM_NCACTIVATE",
	"WM_GETDLGCODE",
	"WM_SYNCPAINT",
	"UNKNOWN (137)",
	"UNKNOWN (138)",
	"UNKNOWN (139)",
	"UNKNOWN (140)",
	"UNKNOWN (141)",
	"UNKNOWN (142)",
	"UNKNOWN (143)",
	"UNKNOWN (144)",
	"UNKNOWN (145)",
	"UNKNOWN (146)",
	"UNKNOWN (147)",
	"UNKNOWN (148)",
	"UNKNOWN (149)",
	"UNKNOWN (150)",
	"UNKNOWN (151)",
	"UNKNOWN (152)",
	"UNKNOWN (153)",
	"UNKNOWN (154)",
	"UNKNOWN (155)",
	"UNKNOWN (156)",
	"UNKNOWN (157)",
	"UNKNOWN (158)",
	"UNKNOWN (159)",
	"WM_NCMOUSEMOVE",
	"WM_NCLBUTTONDOWN",
	"WM_NCLBUTTONUP",
	"WM_NCLBUTTONDBLCLK",
	"WM_NCRBUTTONDOWN",
	"WM_NCRBUTTONUP",
	"WM_NCRBUTTONDBLCLK",
	"WM_NCMBUTTONDOWN",
	"WM_NCMBUTTONUP",
	"WM_NCMBUTTONDBLCLK",
	"UNKNOWN (170)",
	"UNKNOWN (171)",
	"UNKNOWN (172)",
	"UNKNOWN (173)",
	"UNKNOWN (174)",
	"UNKNOWN (175)",
	"UNKNOWN (176)",
	"UNKNOWN (177)",
	"UNKNOWN (178)",
	"UNKNOWN (179)",
	"UNKNOWN (180)",
	"UNKNOWN (181)",
	"UNKNOWN (182)",
	"UNKNOWN (183)",
	"UNKNOWN (184)",
	"UNKNOWN (185)",
	"UNKNOWN (186)",
	"UNKNOWN (187)",
	"UNKNOWN (188)",
	"UNKNOWN (189)",
	"UNKNOWN (190)",
	"UNKNOWN (191)",
	"UNKNOWN (192)",
	"UNKNOWN (193)",
	"UNKNOWN (194)",
	"UNKNOWN (195)",
	"UNKNOWN (196)",
	"UNKNOWN (197)",
	"UNKNOWN (198)",
	"UNKNOWN (199)",
	"UNKNOWN (200)",
	"UNKNOWN (201)",
	"UNKNOWN (202)",
	"UNKNOWN (203)",
	"UNKNOWN (204)",
	"UNKNOWN (205)",
	"UNKNOWN (206)",
	"UNKNOWN (207)",
	"UNKNOWN (208)",
	"UNKNOWN (209)",
	"UNKNOWN (210)",
	"UNKNOWN (211)",
	"UNKNOWN (212)",
	"UNKNOWN (213)",
	"UNKNOWN (214)",
	"UNKNOWN (215)",
	"UNKNOWN (216)",
	"UNKNOWN (217)",
	"UNKNOWN (218)",
	"UNKNOWN (219)",
	"UNKNOWN (220)",
	"UNKNOWN (221)",
	"UNKNOWN (222)",
	"UNKNOWN (223)",
	"UNKNOWN (224)",
	"UNKNOWN (225)",
	"UNKNOWN (226)",
	"UNKNOWN (227)",
	"UNKNOWN (228)",
	"UNKNOWN (229)",
	"UNKNOWN (230)",
	"UNKNOWN (231)",
	"UNKNOWN (232)",
	"UNKNOWN (233)",
	"UNKNOWN (234)",
	"UNKNOWN (235)",
	"UNKNOWN (236)",
	"UNKNOWN (237)",
	"UNKNOWN (238)",
	"UNKNOWN (239)",
	"UNKNOWN (240)",
	"UNKNOWN (241)",
	"UNKNOWN (242)",
	"UNKNOWN (243)",
	"UNKNOWN (244)",
	"UNKNOWN (245)",
	"UNKNOWN (246)",
	"UNKNOWN (247)",
	"UNKNOWN (248)",
	"UNKNOWN (249)",
	"UNKNOWN (250)",
	"UNKNOWN (251)",
	"UNKNOWN (252)",
	"UNKNOWN (253)",
	"UNKNOWN (254)",
	"UNKNOWN (255)",
	"WM_KEYDOWN",
	"WM_KEYUP",
	"WM_CHAR",
	"WM_DEADCHAR",
	"WM_SYSKEYDOWN",
	"WM_SYSKEYUP",
	"WM_SYSCHAR",
	"WM_SYSDEADCHAR",
	"WM_KEYLAST",
	"UNKNOWN (265)",
	"UNKNOWN (266)",
	"UNKNOWN (267)",
	"UNKNOWN (268)",
	"UNKNOWN (269)",
	"UNKNOWN (270)",
	"UNKNOWN (271)",
	"WM_INITDIALOG",
	"WM_COMMAND",
	"WM_SYSCOMMAND",
	"WM_TIMER",
	"WM_HSCROLL",
	"WM_VSCROLL",
	"WM_INITMENU",
	"WM_INITMENUPOPUP",
	"UNKNOWN (280)",
	"UNKNOWN (281)",
	"UNKNOWN (282)",
	"UNKNOWN (283)",
	"UNKNOWN (284)",
	"UNKNOWN (285)",
	"UNKNOWN (286)",
	"WM_MENUSELECT",
	"WM_MENUCHAR",
	"WM_ENTERIDLE",
	"WM_MENURBUTTONUP",
	"WM_MENUDRAG",
	"WM_MENUGETOBJECT",
	"WM_UNINITMENUPOPUP",
	"WM_MENUCOMMAND",
	"UNKNOWN (295)",
	"UNKNOWN (296)",
	"UNKNOWN (297)",
	"UNKNOWN (298)",
	"UNKNOWN (299)",
	"UNKNOWN (300)",
	"UNKNOWN (301)",
	"UNKNOWN (302)",
	"UNKNOWN (303)",
	"UNKNOWN (304)",
	"UNKNOWN (305)",
	"WM_CTLCOLORMSGBOX",
	"WM_CTLCOLOREDIT",
	"WM_CTLCOLORLISTBOX",
	"WM_CTLCOLORBTN",
	"WM_CTLCOLORDLG",
	"WM_CTLCOLORSCROLLBAR",
	"WM_CTLCOLORSTATIC",
	"UNKNOWN (313)",
	"UNKNOWN (314)",
	"UNKNOWN (315)",
	"UNKNOWN (316)",
	"UNKNOWN (317)",
	"UNKNOWN (318)",
	"UNKNOWN (319)",
	"UNKNOWN (320)",
	"UNKNOWN (321)",
	"UNKNOWN (322)",
	"UNKNOWN (323)",
	"UNKNOWN (324)",
	"UNKNOWN (325)",
	"UNKNOWN (326)",
	"UNKNOWN (327)",
	"UNKNOWN (328)",
	"UNKNOWN (329)",
	"UNKNOWN (330)",
	"UNKNOWN (331)",
	"UNKNOWN (332)",
	"UNKNOWN (333)",
	"UNKNOWN (334)",
	"UNKNOWN (335)",
	"UNKNOWN (336)",
	"UNKNOWN (337)",
	"UNKNOWN (338)",
	"UNKNOWN (339)",
	"UNKNOWN (340)",
	"UNKNOWN (341)",
	"UNKNOWN (342)",
	"UNKNOWN (343)",
	"UNKNOWN (344)",
	"UNKNOWN (345)",
	"UNKNOWN (346)",
	"UNKNOWN (347)",
	"UNKNOWN (348)",
	"UNKNOWN (349)",
	"UNKNOWN (350)",
	"UNKNOWN (351)",
	"UNKNOWN (352)",
	"UNKNOWN (353)",
	"UNKNOWN (354)",
	"UNKNOWN (355)",
	"UNKNOWN (356)",
	"UNKNOWN (357)",
	"UNKNOWN (358)",
	"UNKNOWN (359)",
	"UNKNOWN (360)",
	"UNKNOWN (361)",
	"UNKNOWN (362)",
	"UNKNOWN (363)",
	"UNKNOWN (364)",
	"UNKNOWN (365)",
	"UNKNOWN (366)",
	"UNKNOWN (367)",
	"UNKNOWN (368)",
	"UNKNOWN (369)",
	"UNKNOWN (370)",
	"UNKNOWN (371)",
	"UNKNOWN (372)",
	"UNKNOWN (373)",
	"UNKNOWN (374)",
	"UNKNOWN (375)",
	"UNKNOWN (376)",
	"UNKNOWN (377)",
	"UNKNOWN (378)",
	"UNKNOWN (379)",
	"UNKNOWN (380)",
	"UNKNOWN (381)",
	"UNKNOWN (382)",
	"UNKNOWN (383)",
	"UNKNOWN (384)",
	"UNKNOWN (385)",
	"UNKNOWN (386)",
	"UNKNOWN (387)",
	"UNKNOWN (388)",
	"UNKNOWN (389)",
	"UNKNOWN (390)",
	"UNKNOWN (391)",
	"UNKNOWN (392)",
	"UNKNOWN (393)",
	"UNKNOWN (394)",
	"UNKNOWN (395)",
	"UNKNOWN (396)",
	"UNKNOWN (397)",
	"UNKNOWN (398)",
	"UNKNOWN (399)",
	"UNKNOWN (400)",
	"UNKNOWN (401)",
	"UNKNOWN (402)",
	"UNKNOWN (403)",
	"UNKNOWN (404)",
	"UNKNOWN (405)",
	"UNKNOWN (406)",
	"UNKNOWN (407)",
	"UNKNOWN (408)",
	"UNKNOWN (409)",
	"UNKNOWN (410)",
	"UNKNOWN (411)",
	"UNKNOWN (412)",
	"UNKNOWN (413)",
	"UNKNOWN (414)",
	"UNKNOWN (415)",
	"UNKNOWN (416)",
	"UNKNOWN (417)",
	"UNKNOWN (418)",
	"UNKNOWN (419)",
	"UNKNOWN (420)",
	"UNKNOWN (421)",
	"UNKNOWN (422)",
	"UNKNOWN (423)",
	"UNKNOWN (424)",
	"UNKNOWN (425)",
	"UNKNOWN (426)",
	"UNKNOWN (427)",
	"UNKNOWN (428)",
	"UNKNOWN (429)",
	"UNKNOWN (430)",
	"UNKNOWN (431)",
	"UNKNOWN (432)",
	"UNKNOWN (433)",
	"UNKNOWN (434)",
	"UNKNOWN (435)",
	"UNKNOWN (436)",
	"UNKNOWN (437)",
	"UNKNOWN (438)",
	"UNKNOWN (439)",
	"UNKNOWN (440)",
	"UNKNOWN (441)",
	"UNKNOWN (442)",
	"UNKNOWN (443)",
	"UNKNOWN (444)",
	"UNKNOWN (445)",
	"UNKNOWN (446)",
	"UNKNOWN (447)",
	"UNKNOWN (448)",
	"UNKNOWN (449)",
	"UNKNOWN (450)",
	"UNKNOWN (451)",
	"UNKNOWN (452)",
	"UNKNOWN (453)",
	"UNKNOWN (454)",
	"UNKNOWN (455)",
	"UNKNOWN (456)",
	"UNKNOWN (457)",
	"UNKNOWN (458)",
	"UNKNOWN (459)",
	"UNKNOWN (460)",
	"UNKNOWN (461)",
	"UNKNOWN (462)",
	"UNKNOWN (463)",
	"UNKNOWN (464)",
	"UNKNOWN (465)",
	"UNKNOWN (466)",
	"UNKNOWN (467)",
	"UNKNOWN (468)",
	"UNKNOWN (469)",
	"UNKNOWN (470)",
	"UNKNOWN (471)",
	"UNKNOWN (472)",
	"UNKNOWN (473)",
	"UNKNOWN (474)",
	"UNKNOWN (475)",
	"UNKNOWN (476)",
	"UNKNOWN (477)",
	"UNKNOWN (478)",
	"UNKNOWN (479)",
	"UNKNOWN (480)",
	"UNKNOWN (481)",
	"UNKNOWN (482)",
	"UNKNOWN (483)",
	"UNKNOWN (484)",
	"UNKNOWN (485)",
	"UNKNOWN (486)",
	"UNKNOWN (487)",
	"UNKNOWN (488)",
	"UNKNOWN (489)",
	"UNKNOWN (490)",
	"UNKNOWN (491)",
	"UNKNOWN (492)",
	"UNKNOWN (493)",
	"UNKNOWN (494)",
	"UNKNOWN (495)",
	"UNKNOWN (496)",
	"UNKNOWN (497)",
	"UNKNOWN (498)",
	"UNKNOWN (499)",
	"UNKNOWN (500)",
	"UNKNOWN (501)",
	"UNKNOWN (502)",
	"UNKNOWN (503)",
	"UNKNOWN (504)",
	"UNKNOWN (505)",
	"UNKNOWN (506)",
	"UNKNOWN (507)",
	"UNKNOWN (508)",
	"UNKNOWN (509)",
	"UNKNOWN (510)",
	"UNKNOWN (511)",
	"WM_MOUSEMOVE",
	"WM_LBUTTONDOWN",
	"WM_LBUTTONUP",
	"WM_LBUTTONDBLCLK",
	"WM_RBUTTONDOWN",
	"WM_RBUTTONUP",
	"WM_RBUTTONDBLCLK",
	"WM_MBUTTONDOWN",
	"WM_MBUTTONUP",
	"WM_MOUSELAST",
	"WM_MOUSEWHEEL",
	"WM_XBUTTONDOWN",
	"WM_XBUTTONUP",
	"UNKNOWN (525)",
	"UNKNOWN (526)",
	"UNKNOWN (527)",
	"WM_PARENTNOTIFY",
	"WM_ENTERMENULOOP",
	"WM_EXITMENULOOP",
	"WM_NEXTMENU",
	"WM_SIZING",
	"WM_CAPTURECHANGED",
	"WM_MOVING",
	"UNKNOWN (535)",
	"WM_POWERBROADCAST",
	"WM_DEVICECHANGE",
	"UNKNOWN (538)",
	"UNKNOWN (539)",
	"UNKNOWN (540)",
	"UNKNOWN (541)",
	"UNKNOWN (542)",
	"UNKNOWN (543)",
	"WM_MDICREATE",
	"WM_MDIDESTROY",
	"WM_MDIACTIVATE",
	"WM_MDIRESTORE",
	"WM_MDINEXT",
	"WM_MDIMAXIMIZE",
	"WM_MDITILE",
	"WM_MDICASCADE",
	"WM_MDIICONARRANGE",
	"WM_MDIGETACTIVE",
	"UNKNOWN (554)",
	"UNKNOWN (555)",
	"UNKNOWN (556)",
	"UNKNOWN (557)",
	"UNKNOWN (558)",
	"UNKNOWN (559)",
	"WM_MDISETMENU",
	"WM_ENTERSIZEMOVE",
	"WM_EXITSIZEMOVE",
	"WM_DROPFILES",
	"WM_MDIREFRESHMENU",
	"UNKNOWN (565)",
	"UNKNOWN (566)",
	"UNKNOWN (567)",
	"UNKNOWN (568)",
	"UNKNOWN (569)",
	"UNKNOWN (570)",
	"UNKNOWN (571)",
	"UNKNOWN (572)",
	"UNKNOWN (573)",
	"UNKNOWN (574)",
	"UNKNOWN (575)",
	"UNKNOWN (576)",
	"UNKNOWN (577)",
	"UNKNOWN (578)",
	"UNKNOWN (579)",
	"UNKNOWN (580)",
	"UNKNOWN (581)",
	"UNKNOWN (582)",
	"UNKNOWN (583)",
	"UNKNOWN (584)",
	"UNKNOWN (585)",
	"UNKNOWN (586)",
	"UNKNOWN (587)",
	"UNKNOWN (588)",
	"UNKNOWN (589)",
	"UNKNOWN (590)",
	"UNKNOWN (591)",
	"UNKNOWN (592)",
	"UNKNOWN (593)",
	"UNKNOWN (594)",
	"UNKNOWN (595)",
	"UNKNOWN (596)",
	"UNKNOWN (597)",
	"UNKNOWN (598)",
	"UNKNOWN (599)",
	"UNKNOWN (600)",
	"UNKNOWN (601)",
	"UNKNOWN (602)",
	"UNKNOWN (603)",
	"UNKNOWN (604)",
	"UNKNOWN (605)",
	"UNKNOWN (606)",
	"UNKNOWN (607)",
	"UNKNOWN (608)",
	"UNKNOWN (609)",
	"UNKNOWN (610)",
	"UNKNOWN (611)",
	"UNKNOWN (612)",
	"UNKNOWN (613)",
	"UNKNOWN (614)",
	"UNKNOWN (615)",
	"UNKNOWN (616)",
	"UNKNOWN (617)",
	"UNKNOWN (618)",
	"UNKNOWN (619)",
	"UNKNOWN (620)",
	"UNKNOWN (621)",
	"UNKNOWN (622)",
	"UNKNOWN (623)",
	"UNKNOWN (624)",
	"UNKNOWN (625)",
	"UNKNOWN (626)",
	"UNKNOWN (627)",
	"UNKNOWN (628)",
	"UNKNOWN (629)",
	"UNKNOWN (630)",
	"UNKNOWN (631)",
	"UNKNOWN (632)",
	"UNKNOWN (633)",
	"UNKNOWN (634)",
	"UNKNOWN (635)",
	"UNKNOWN (636)",
	"UNKNOWN (637)",
	"UNKNOWN (638)",
	"UNKNOWN (639)",
	"UNKNOWN (640)",
	"UNKNOWN (641)",
	"UNKNOWN (642)",
	"UNKNOWN (643)",
	"UNKNOWN (644)",
	"UNKNOWN (645)",
	"UNKNOWN (646)",
	"UNKNOWN (647)",
	"UNKNOWN (648)",
	"UNKNOWN (649)",
	"UNKNOWN (650)",
	"UNKNOWN (651)",
	"UNKNOWN (652)",
	"UNKNOWN (653)",
	"UNKNOWN (654)",
	"UNKNOWN (655)",
	"UNKNOWN (656)",
	"UNKNOWN (657)",
	"UNKNOWN (658)",
	"UNKNOWN (659)",
	"UNKNOWN (660)",
	"UNKNOWN (661)",
	"UNKNOWN (662)",
	"UNKNOWN (663)",
	"UNKNOWN (664)",
	"UNKNOWN (665)",
	"UNKNOWN (666)",
	"UNKNOWN (667)",
	"UNKNOWN (668)",
	"UNKNOWN (669)",
	"UNKNOWN (670)",
	"UNKNOWN (671)",
	"UNKNOWN (672)",
	"WM_MOUSEHOVER",
	"UNKNOWN (674)",
	"WM_MOUSELEAVE",
	"UNKNOWN (676)",
	"UNKNOWN (677)",
	"UNKNOWN (678)",
	"UNKNOWN (679)",
	"UNKNOWN (680)",
	"UNKNOWN (681)",
	"UNKNOWN (682)",
	"UNKNOWN (683)",
	"UNKNOWN (684)",
	"UNKNOWN (685)",
	"UNKNOWN (686)",
	"UNKNOWN (687)",
	"UNKNOWN (688)",
	"UNKNOWN (689)",
	"UNKNOWN (690)",
	"UNKNOWN (691)",
	"UNKNOWN (692)",
	"UNKNOWN (693)",
	"UNKNOWN (694)",
	"UNKNOWN (695)",
	"UNKNOWN (696)",
	"UNKNOWN (697)",
	"UNKNOWN (698)",
	"UNKNOWN (699)",
	"UNKNOWN (700)",
	"UNKNOWN (701)",
	"UNKNOWN (702)",
	"UNKNOWN (703)",
	"UNKNOWN (704)",
	"UNKNOWN (705)",
	"UNKNOWN (706)",
	"UNKNOWN (707)",
	"UNKNOWN (708)",
	"UNKNOWN (709)",
	"UNKNOWN (710)",
	"UNKNOWN (711)",
	"UNKNOWN (712)",
	"UNKNOWN (713)",
	"UNKNOWN (714)",
	"UNKNOWN (715)",
	"UNKNOWN (716)",
	"UNKNOWN (717)",
	"UNKNOWN (718)",
	"UNKNOWN (719)",
	"UNKNOWN (720)",
	"UNKNOWN (721)",
	"UNKNOWN (722)",
	"UNKNOWN (723)",
	"UNKNOWN (724)",
	"UNKNOWN (725)",
	"UNKNOWN (726)",
	"UNKNOWN (727)",
	"UNKNOWN (728)",
	"UNKNOWN (729)",
	"UNKNOWN (730)",
	"UNKNOWN (731)",
	"UNKNOWN (732)",
	"UNKNOWN (733)",
	"UNKNOWN (734)",
	"UNKNOWN (735)",
	"UNKNOWN (736)",
	"UNKNOWN (737)",
	"UNKNOWN (738)",
	"UNKNOWN (739)",
	"UNKNOWN (740)",
	"UNKNOWN (741)",
	"UNKNOWN (742)",
	"UNKNOWN (743)",
	"UNKNOWN (744)",
	"UNKNOWN (745)",
	"UNKNOWN (746)",
	"UNKNOWN (747)",
	"UNKNOWN (748)",
	"UNKNOWN (749)",
	"UNKNOWN (750)",
	"UNKNOWN (751)",
	"UNKNOWN (752)",
	"UNKNOWN (753)",
	"UNKNOWN (754)",
	"UNKNOWN (755)",
	"UNKNOWN (756)",
	"UNKNOWN (757)",
	"UNKNOWN (758)",
	"UNKNOWN (759)",
	"UNKNOWN (760)",
	"UNKNOWN (761)",
	"UNKNOWN (762)",
	"UNKNOWN (763)",
	"UNKNOWN (764)",
	"UNKNOWN (765)",
	"UNKNOWN (766)",
	"UNKNOWN (767)",
	"WM_CUT",
	"WM_COPY",
	"WM_PASTE",
	"WM_CLEAR",
	"WM_UNDO",
	"WM_RENDERFORMAT",
	"WM_RENDERALLFORMATS",
	"WM_DESTROYCLIPBOARD",
	"WM_DRAWCLIPBOARD",
	"WM_PAINTCLIPBOARD",
	"WM_VSCROLLCLIPBOARD",
	"WM_SIZECLIPBOARD",
	"WM_ASKCBFORMATNAME",
	"WM_CHANGECBCHAIN",
	"WM_HSCROLLCLIPBOARD",
	"WM_QUERYNEWPALETTE",
	"WM_PALETTEISCHANGING",
	"WM_PALETTECHANGED",
	"WM_HOTKEY",
	"UNKNOWN (787)",
	"UNKNOWN (788)",
	"UNKNOWN (789)",
	"UNKNOWN (790)",
	"WM_PRINT",
	"WM_PRINTCLIENT",
	"UNKNOWN (793)",
	"UNKNOWN (794)",
	"UNKNOWN (795)",
	"UNKNOWN (796)",
	"UNKNOWN (797)",
	"UNKNOWN (798)",
	"UNKNOWN (799)",
	"UNKNOWN (800)",
	"UNKNOWN (801)",
	"UNKNOWN (802)",
	"UNKNOWN (803)",
	"UNKNOWN (804)",
	"UNKNOWN (805)",
	"UNKNOWN (806)",
	"UNKNOWN (807)",
	"UNKNOWN (808)",
	"UNKNOWN (809)",
	"UNKNOWN (810)",
	"UNKNOWN (811)",
	"UNKNOWN (812)",
	"UNKNOWN (813)",
	"UNKNOWN (814)",
	"UNKNOWN (815)",
	"UNKNOWN (816)",
	"UNKNOWN (817)",
	"UNKNOWN (818)",
	"UNKNOWN (819)",
	"UNKNOWN (820)",
	"UNKNOWN (821)",
	"UNKNOWN (822)",
	"UNKNOWN (823)",
	"UNKNOWN (824)",
	"UNKNOWN (825)",
	"UNKNOWN (826)",
	"UNKNOWN (827)",
	"UNKNOWN (828)",
	"UNKNOWN (829)",
	"UNKNOWN (830)",
	"UNKNOWN (831)",
	"UNKNOWN (832)",
	"UNKNOWN (833)",
	"UNKNOWN (834)",
	"UNKNOWN (835)",
	"UNKNOWN (836)",
	"UNKNOWN (837)",
	"UNKNOWN (838)",
	"UNKNOWN (839)",
	"UNKNOWN (840)",
	"UNKNOWN (841)",
	"UNKNOWN (842)",
	"UNKNOWN (843)",
	"UNKNOWN (844)",
	"UNKNOWN (845)",
	"UNKNOWN (846)",
	"UNKNOWN (847)",
	"UNKNOWN (848)",
	"UNKNOWN (849)",
	"UNKNOWN (850)",
	"UNKNOWN (851)",
	"UNKNOWN (852)",
	"UNKNOWN (853)",
	"UNKNOWN (854)",
	"UNKNOWN (855)",
	"WM_HANDHELDFIRST",
	"UNKNOWN (857)",
	"UNKNOWN (858)",
	"UNKNOWN (859)",
	"UNKNOWN (860)",
	"UNKNOWN (861)",
	"UNKNOWN (862)",
	"WM_HANDHELDLAST",
	"WM_AFXFIRST",
	"UNKNOWN (865)",
	"UNKNOWN (866)",
	"UNKNOWN (867)",
	"UNKNOWN (868)",
	"UNKNOWN (869)",
	"UNKNOWN (870)",
	"UNKNOWN (871)",
	"UNKNOWN (872)",
	"UNKNOWN (873)",
	"UNKNOWN (874)",
	"UNKNOWN (875)",
	"UNKNOWN (876)",
	"UNKNOWN (877)",
	"UNKNOWN (878)",
	"UNKNOWN (879)",
	"UNKNOWN (880)",
	"UNKNOWN (881)",
	"UNKNOWN (882)",
	"UNKNOWN (883)",
	"UNKNOWN (884)",
	"UNKNOWN (885)",
	"UNKNOWN (886)",
	"UNKNOWN (887)",
	"UNKNOWN (888)",
	"UNKNOWN (889)",
	"UNKNOWN (890)",
	"UNKNOWN (891)",
	"UNKNOWN (892)",
	"UNKNOWN (893)",
	"UNKNOWN (894)",
	"WM_AFXLAST",
	"WM_PENWINFIRST",
	"UNKNOWN (897)",
	"UNKNOWN (898)",
	"UNKNOWN (899)",
	"UNKNOWN (900)",
	"UNKNOWN (901)",
	"UNKNOWN (902)",
	"UNKNOWN (903)",
	"UNKNOWN (904)",
	"UNKNOWN (905)",
	"UNKNOWN (906)",
	"UNKNOWN (907)",
	"UNKNOWN (908)",
	"UNKNOWN (909)",
	"UNKNOWN (910)",
	"WM_PENWINLAST",
	"UNKNOWN (912)",
	"UNKNOWN (913)",
	"UNKNOWN (914)",
	"UNKNOWN (915)",
	"UNKNOWN (916)",
	"UNKNOWN (917)",
	"UNKNOWN (918)",
	"UNKNOWN (919)",
	"UNKNOWN (920)",
	"UNKNOWN (921)",
	"UNKNOWN (922)",
	"UNKNOWN (923)",
	"UNKNOWN (924)",
	"UNKNOWN (925)",
	"UNKNOWN (926)",
	"UNKNOWN (927)",
	"UNKNOWN (928)",
	"UNKNOWN (929)",
	"UNKNOWN (930)",
	"UNKNOWN (931)",
	"UNKNOWN (932)",
	"UNKNOWN (933)",
	"UNKNOWN (934)",
	"UNKNOWN (935)",
	"UNKNOWN (936)",
	"UNKNOWN (937)",
	"UNKNOWN (938)",
	"UNKNOWN (939)",
	"UNKNOWN (940)",
	"UNKNOWN (941)",
	"UNKNOWN (942)",
	"UNKNOWN (943)",
	"UNKNOWN (944)",
	"UNKNOWN (945)",
	"UNKNOWN (946)",
	"UNKNOWN (947)",
	"UNKNOWN (948)",
	"UNKNOWN (949)",
	"UNKNOWN (950)",
	"UNKNOWN (951)",
	"UNKNOWN (952)",
	"UNKNOWN (953)",
	"UNKNOWN (954)",
	"UNKNOWN (955)",
	"UNKNOWN (956)",
	"UNKNOWN (957)",
	"UNKNOWN (958)",
	"UNKNOWN (959)",
	"UNKNOWN (960)",
	"UNKNOWN (961)",
	"UNKNOWN (962)",
	"UNKNOWN (963)",
	"UNKNOWN (964)",
	"UNKNOWN (965)",
	"UNKNOWN (966)",
	"UNKNOWN (967)",
	"UNKNOWN (968)",
	"UNKNOWN (969)",
	"UNKNOWN (970)",
	"UNKNOWN (971)",
	"UNKNOWN (972)",
	"UNKNOWN (973)",
	"UNKNOWN (974)",
	"UNKNOWN (975)",
	"UNKNOWN (976)",
	"UNKNOWN (977)",
	"UNKNOWN (978)",
	"UNKNOWN (979)",
	"UNKNOWN (980)",
	"UNKNOWN (981)",
	"UNKNOWN (982)",
	"UNKNOWN (983)",
	"UNKNOWN (984)",
	"UNKNOWN (985)",
	"UNKNOWN (986)",
	"UNKNOWN (987)",
	"UNKNOWN (988)",
	"UNKNOWN (989)",
	"UNKNOWN (990)",
	"UNKNOWN (991)",
	"UNKNOWN (992)",
	"UNKNOWN (993)",
	"UNKNOWN (994)",
	"UNKNOWN (995)",
	"UNKNOWN (996)",
	"UNKNOWN (997)",
	"UNKNOWN (998)",
	"UNKNOWN (999)",
	"UNKNOWN (1000)",
	"UNKNOWN (1001)",
	"UNKNOWN (1002)",
	"UNKNOWN (1003)",
	"UNKNOWN (1004)",
	"UNKNOWN (1005)",
	"UNKNOWN (1006)",
	"UNKNOWN (1007)",
	"UNKNOWN (1008)",
	"UNKNOWN (1009)",
	"UNKNOWN (1010)",
	"UNKNOWN (1011)",
	"UNKNOWN (1012)",
	"UNKNOWN (1013)",
	"UNKNOWN (1014)",
	"UNKNOWN (1015)",
	"UNKNOWN (1016)",
	"UNKNOWN (1017)",
	"UNKNOWN (1018)",
	"UNKNOWN (1019)",
	"UNKNOWN (1020)",
	"UNKNOWN (1021)",
	"UNKNOWN (1022)",
	"UNKNOWN (1023)",
	"WM_USER"
};
#endif  // WM_DEBUG


#define TKVST2_MAX_WINDOWTITLE_SIZE 256

class TKVST2Window {
public:
   TKVST2Window *next;
   VST2Plugin   *plugin;
   HWND          hWnd;
   char          window_title[TKVST2_MAX_WINDOWTITLE_SIZE];
   sBool         b_allow_redirect_close_to_hide;
};

static TKVST2Window *first_window = NULL;

static char init_windowTitle[TKVST2_MAX_WINDOWTITLE_SIZE];
static void BuildWindowTitle (VST2Plugin *thiz);
static TKVST2Window *CreateVST2Window (VST2Plugin *thiz, sBool bAddWindow, sBool bShowWindow);
static TKVST2Window *InitVST2Window (VST2Plugin *thiz, HWND hWnd, sBool bAddWindow, sBool bShowWindow);

static TKVST2Window *FindVST2WindowByHandle(HWND hWnd) {

   // yac_host->printf("xxx FindVST2WindowByHandle 1\n");
   LockWindows();
   // yac_host->printf("xxx FindVST2WindowByHandle 2\n");

   TKVST2Window *vw = first_window;
   while(NULL != vw)
   {
      if(vw->hWnd == hWnd)
      {
         break;
      }
      else
      {
         vw = vw->next;
      }
   }

   // yac_host->printf("xxx FindVST2WindowByHandle 3\n");

   UnlockWindows();
   // yac_host->printf("xxx FindVST2WindowByHandle 4\n");

   return vw;
}

static TKVST2Window *FindVST2WindowByPlugin(VST2Plugin *_plugin, sBool _bLock) {

   if(_bLock)
   {
      LockWindows();
   }

   TKVST2Window *vw = first_window;
   while(NULL != vw)
   {
      if(vw->plugin == _plugin)
      {
         break;
      }
      else
      {
         vw = vw->next;
      }
   }

   if(_bLock)
   {
      UnlockWindows();
   }

   return vw;
}

static void RemoveVST2WindowByHandle(HWND hWnd) {

   // called by WM_CLOSE

   yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: hWnd=%p\n", hWnd);

   // // LockWindows();

   TKVST2Window *vw = first_window;
   while(NULL != vw)
   {
      if(vw->hWnd == hWnd)
      {
         // Remember window position and size
         {
            yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: caching window geometry\n");
            RECT rect;
            ::GetWindowRect(hWnd, &rect);
            vw->plugin->ui_window_left   = rect.left;
            vw->plugin->ui_window_top    = rect.top;
            vw->plugin->ui_window_right  = rect.right;
            vw->plugin->ui_window_bottom = rect.bottom;
            yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: rect=(%d; %d; %d; %d)\n",
                   rect.left,
                   rect.top,
                   rect.right,
                   rect.bottom
                   );
         }

         yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: calling effEditClose\n");

         // Notify plugin
         vw->plugin->beginDispatch();
         vw->plugin->effect->dispatcher(vw->plugin->effect, effEditClose, 0, 0, NULL, 0.0f);
         vw->plugin->endDispatch();

         yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: effEditClose returned\n");

         // Close/destroy window
         ::DestroyWindow(hWnd);

         yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: window hWnd=0x%p destroyed\n", hWnd);


         // Unregister class

         LockWindows();

         // HINSTANCE hInstance = vw->plugin->dll_handle;
         HINSTANCE hInstance = ::GetModuleHandle(NULL);

         yac_host->printf("[dbg] tkvst2::RemoveVST2WindowByHandle: UnregisterClass(\"%s\")\n", vw->window_title);
         if(!::UnregisterClass(vw->window_title, hInstance))
         {
            yac_host->printf("[---] RemoveVST2WindowByHandle: UnregisterClass() failed. LastError=%d\n", ::GetLastError());
         }

         // Remove from window list
         if(vw == first_window)
         {
            // Unlink from window list (first window)
            first_window = vw->next;
         }
         else
         {
            // Unlink from window list
            TKVST2Window *p = first_window;
            while(p->next != vw)
            {
               p = p->next;
            }
            p->next = vw->next;
         }

         // Free memory for TKVST2Window
         delete vw;

         UnlockWindows();

         break;
      }
      else
      {
         vw = vw->next;
      }
   }

   // // UnlockWindows();
}



typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

static VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt) {
   static VstInt32 lastOpcode = -1;
   static VstIntPtr lastTimeMask = ~0;
	VstIntPtr result = 0;
   VST2Plugin *plugin;

   if(NULL != effect)
   {
      plugin = (VST2Plugin*) effect->resvd1;
   }
   else
   {
      plugin = NULL;
   }

   // printf("xxx audioMasterGetTime=%d\n", audioMasterGetTime); // ==> 7

   // opcode 15: when Redux editor button is clicked to resize the window back to minimum
   //  opcode=15 index=868 value=0x00000000000000A1

   if( (44 != opcode) && (43 != opcode) && (3 != opcode) && (6 != opcode) && (4 != opcode) && (8 != opcode) && (7 != opcode) && (23 != opcode) && (0 != opcode) && (37 != opcode) && (33 != opcode) ) // filter audioMasterGetTime, audioMasterGetProcessLevel, audioMasterAutomate, audioMasterCanDo
   {
      if(1 && (lastOpcode != opcode))
         Dopprintf("[dbg] VST2Plugin::HostCallback: effect=0x%p opcode=%d index=%d value=0x%p ptr=0x%p opt=%f\n", effect, opcode, index, value, ptr, opt);
   }

   // (note) opcode documentation borrowed from <http://www.asseca.com/vst-24-specs/amCallbacks.html> ([20Jan2018] site is gone)

   switch(opcode)
   {
      case audioMasterAutomate: // 0
      {
         // plugin notifies host that the plugin has changed a parameter value thru its GUI or MIDI.
         // e[index]: parameter index
         // e[opt]: parameter value
         // x[return]: 0
         if(lastOpcode != audioMasterAutomate)
         {
            ////yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterAutomate\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterAutomate(index, opt);
         }
      }
      break;
         
      case audioMasterVersion: // 1, Allowed before VstPluginMain returns
         // plugin requests host VST-specification version, currently 2 (0 for older)
         // x[return]: Host VST version (for example 2400 for VST2.4)
         //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterVersion\n");
         // (note) Arturia Synclavier V expects "2" (else it won't load)
         result = 2;//kVstVersion;
         break;

      case audioMasterCurrentId: // 2
         // plugin requests host for current unique identifier
         // x[return]: current unique ID of a shell plug-in (in VST2.3 x[return]: the unique ID of the plug that is currently loaded)
         yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterCurrentId\n");
         if(NULL != effect)
         {
            if(NULL != plugin)
            {
               if(0u != plugin->shell_plugin_uid)
               {
                  Dopprintf("[dbg] tkvst2::HostCallback<audioMasterCurrentId>: return shell_plugin_uid=0x%08x\n", plugin->shell_plugin_uid);
                  result = plugin->shell_plugin_uid;
               }
               else
               {
                  result = effect->uniqueID;
               }
            }
            else if(0u != VST2Plugin::static_shell_plugin_uid)
            {
               Dopprintf("[dbg] tksvst2::HostCallback<audioMasterCurrentId>: return static_shell_plugin_uid=0x%08x\n", (sUI)VST2Plugin::static_shell_plugin_uid);
               result = VST2Plugin::static_shell_plugin_uid;
            }
            else
            {
               result = effect->uniqueID;
            }
         }
         break;

      case audioMasterIdle: // 3
         // plugin calls host idle routine (this will call effEditIdle for all open editors too)
         // has no arguments
         if(lastOpcode != opcode)
         {
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterIdle (Subsequent calls will not be displayed!)");
         }
         break;

      case audioMasterPinConnected: // 4, **DEPRECATED**
         // plugin inquires host if an input or output is being connected
         // e[index]: enumerates input or output counting from zero
         // e[value]: 0 = input, else = output
         // x[return]: the return value is 0 for <true> such that older versions will always return true.
         if(lastOpcode != opcode)
         {
            // (note) GForce Oddity2 spams _a lot_ of these callbacks
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterPinConnected (**deprecated**)\n");
         }
         break;

         // ---- AEffectX:

      case audioMasterWantMidi: // 6, **DEPRECATED** . Allowed before VstPluginMain returns
         // (might need to be re-enabled to ensure 2.3/2.4 compatibility)
         // plugin (VSTi2.0 thru VSTi2.3) calls this to tell the host that the plugin is an instrument
         // e[value]: is a filter which is currently ignored
         if(lastOpcode != opcode)
         {
            // yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterWantMidi (**deprecated**)\n");
         }
         result = 1;
         break;

      case audioMasterGetTime: // 7
         // e[value]: request mask, should contain a mask indicating which fields are required (see valid masks above), as some items may require extensive
         // conversions. For valid masks see VstTimeInfo.Flags
         // e[ptr]: returns VstTimeInfo* or null if not supported 

         // (note) Some plugins seem to call this for each sample during processReplacing() (??!!!)

         if((lastOpcode != audioMasterGetTime) || (lastTimeMask != value))
         {
            lastTimeMask = value;
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetTime requestMask=0x%08x\n", value); 
         }

         if(NULL != plugin)
         {
            plugin->time_info.tempo = tkvst2_bpm;
            plugin->time_info.ppqPos = tkvst2_song_pos_ppq;

            // (note) workaround for OB-E v2.0 (cuts off first note after replay has been started)
            sBool bReportTransportPlaying = plugin->b_report_transport_playing;

            if(bReportTransportPlaying)
            {
               if(tkvst2_song_playing)
                  plugin->time_info.flags |= kVstTransportPlaying;
               else
                  plugin->time_info.flags &= ~kVstTransportPlaying;

               if(tkvst2_song_playing != plugin->last_tkvst2_song_playing)
               {
                  plugin->time_info.flags |= kVstTransportChanged;
                  plugin->last_tkvst2_song_playing = tkvst2_song_playing;
                  // yac_host->printf("xxx audioMasterGetTime: kVstTransportChanged (%d)\n", tkvst2_song_playing);
               }
            }
            else
            {
               plugin->time_info.flags &= ~kVstTransportPlaying;
            }

            if(0)
            {
               static int xxx_audioMasterGetTime_count = 0;
               yac_host->printf("xxx audioMasterGetTime: [%d] BPM=%f\n", xxx_audioMasterGetTime_count++, plugin->time_info.tempo);
            }

            result = (VstIntPtr)  &plugin->time_info;
         }
         else
         {
            result = 0;
         }

         break;

      case audioMasterProcessEvents: // 8
         // plugin informs host that the plugin has MIDI events ready for the host to process
         // e[ptr]: pointer to VstEvents*
         // x[return]: 1 = supported and processed OK
         if(lastOpcode != opcode)
         {
            // yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterProcessEvents\n");
         }
         break;

      case audioMasterSetTime: // **DEPRECATED**
         // e[value]: filter
         // e[ptr]: pointer to VstTimenfo*
         if(lastOpcode != opcode)
         {
            yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetTime (**deprecated**)\n");
         }
         break;

      case audioMasterTempoAt: // **DEPRECATED**
         // ( might need to be enabled to ensure 2.3/2.4 compatibility)
         // e[value]: sample frame location
         // x[return]: tempo (in bpm * 10000)
         if(lastOpcode != opcode)
         {
            ////yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterTempoAt (**deprecated**) BPM=%f\n", tkvst2_bpm);
         }
         result = (sSI) (10000 * tkvst2_bpm);
         break;

      case audioMasterGetNumAutomatableParameters: // **DEPRECATED**
         // plugin asks host number of automatable Parameters
         // x[return]: number of automatable Parameters (should be <= than numParams)
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetNumAutomatableParameters (**deprecated**)\n");
         }
         break;

      case audioMasterGetParameterQuantization: // **DEPRECATED**
         // e[value]: parameter index (-1: all, any)
         // x[return]: integer value for +1.0 representation, or 1 if full single float precision is maintained in automation. // connections, configuration
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetParameterQuantization (**deprecated**)\n");
         }
         break;
         
      case audioMasterIOChanged:
      {
         // plugin notifies host the numInputs and/or numOutputs of plugin has changed
         // x[return]: 1 = supported
         Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterIOChanged\n");

         if(NULL != plugin)
         {
            plugin->callOnAudioMasterIOChanged();
            result = 1;
         }
      }
      break;

      case audioMasterNeedIdle: // **DEPRECATED**
         // plugin needs idle calls (WM_TIMER)
         // has no arguments
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterNeedIdle (**deprecated**)\n");
         }
         break;

      case audioMasterSizeWindow:
         // plugin requests host to resize the window of the plugin editor
         // e[index]: new width in pixels
         // e[value]: new height in pizels
         // x[return]: 1 = supported
         // // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSizeWindow, geo=(%d; %d)\n", sSI(index), sSI(value));
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSizeWindow, plugin=%p\n", plugin);

            if(NULL != plugin)
            {
               plugin->queueResizeWindow(sUI(index), sUI(value));
            }
         }
         break;

      case audioMasterGetSampleRate:
         // x[return]: current sample rate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetSampleRate\n");
         }
         break;

      case audioMasterGetBlockSize:
         // x[return]: current block size
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetBlockSize\n");
         }
         break;

      case audioMasterGetInputLatency:
         // plugin requests input latency from host
         // x[return]: input latency in samples
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetInputLatency\n");
         }
         break;

      case audioMasterGetOutputLatency:
         // plugin requests output latency from host
         // x[return]: output latency in samples
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetOutputLatency\n");
         }
         break;

      case audioMasterGetPreviousPlug: // **DEPRECATED**
         // e[value]: input pin (-1: first to come),
         // x[ptr]: host returns pointer to cEffect*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetPreviousPlug (**deprecated**)\n");
         }
         break;

      case audioMasterGetNextPlug: // **DEPRECATED**
         // e[value]: output pin (-1: first to come),
         // x[ptr]: host returns pointer to cEffect*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetNextPlug (**deprecated**)\n");
         }
         break;

      case audioMasterWillReplaceOrAccumulate: // **DEPRECATED**
         // x[return]: 0 = not supported, 1 = replace, 2 = accumulate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterWillReplaceOrAccumulate (**deprecated**)\n");
         }
         break;

      case audioMasterGetCurrentProcessLevel:
         // x[return]: currentProcessLevel 
         //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetCurrentProcessLevel\n");
         break;

      case audioMasterGetAutomationState:
         // x[return]: 0 = not supported, 1 = off, 2 = read, 3 = write, 4 = read/write
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetAutomationState\n");
         }
         break;

      case audioMasterOfflineStart:
         // e[index]: number of new audio files to start
         // e[value]: number of audio files to start
         // e[ptr]: pointer to *audioFiles
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineStart\n");
         }
         break;

      case audioMasterOfflineRead:
         // e[index]: 1 = readSource
         // e[value]: option of type VstOfflineOption
         // e[ptr]: points to offline* offline structure, see below.
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineRead\n");
         }
         break;

      case audioMasterOfflineWrite:
         // e[index]: 1 = readSource
         // e[value]: option of type VstOfflineOption
         // e[ptr]: points to offline* offline structure, see below.
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineWrite\n");
         }
         break;

      case audioMasterOfflineGetCurrentPass:
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineGetCurrentPass\n");
         }
         break;

      case audioMasterOfflineGetCurrentMetaPass:
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineGetCurrentMetaPass\n");
         }
         break;

      case audioMasterSetOutputSampleRate: // **DEPRECATED**
         // for variable i/o
         // e[opt]: sample rate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetOutputSampleRate (**deprecated**)\n");
         }
         break;

      case audioMasterGetOutputSpeakerArrangement: // **DEPRECATED**
         // x[return]: host returns a pointer to VstSpeakerArrangement* structure
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetOutputSpeakerArrangement (**deprecated**)\n");
         }
         break;

      case audioMasterGetVendorString: // 32, Allowed before VstPluginMain returns
         // x[ptr]: host returns a pointer to a string identifying the vendor (max 64 char)
         // x[return]: 0 = error, 1 = OK
         // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetVendorString\n");
            char *d = (char*)ptr;
            ::snprintf(d, 32, "Unknown");
         }
         break;

      case audioMasterGetProductString: // Allowed before VstPluginMain returns
         // x[ptr]: host returns a pointer to a string with product name (max 64 char)
         // x[return]: 0 = error, 1 = OK
         // if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetProductString\n");
            char *d = (char*)ptr;
            ::snprintf(d, 32, "Unknown");
         }
         break;

      case audioMasterGetVendorVersion: // Allowed before VstPluginMain returns
         // x[return]: vendor-specific version
         // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetVendorVersion\n");
            result = 1;
         }
         break;

      case audioMasterVendorSpecific:
         // no definition, vendor specific handling
         // x[return]: 0 = not supported
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterVendorSpecific\n");
         }
         break;

      case audioMasterSetIcon: // **DEPRECATED**
         // e[ptr]: void*, format not defined yet
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetIcon (**deprecated**)\n");
         }
         break;

      case audioMasterCanDo: // 37
         // plugin queries if host 'cando'
         // e[ptr]: string-pointer to cando string
         // x[returns]: 0 = No, 1='cando', 0='don't know' (default)
         // // if(lastOpcode != opcode)
         {
            // "sendVstEvents",
            // "sendVstMidiEvent"
            // "sendVstTimeInfo"
            if(!strncmp((const char*)ptr, "sendVstTimeInfo", 15))
            {
               result = 1;
            }
            // "receiveVstEvents"
            // "receiveVstMidiEvent"
            // "receiveVstTimeInfo"
            // "reportConnectionChanges"
            // "acceptIOChanges"
            if(!strncmp((const char*)ptr, "sizeWindow", 10))
            {
               result = 1;
            }
            // "asyncProcessing"
            // "offline"
            // "supplyIdle"
            // "supportShell"
            // "openFileSelector"
            // "editFile"
            // "closeFileSelector"
            // "getChunkFile"
            else if(!strncmp((const char*)ptr, "NIMKPIVendorSpecificCallbacks", 29))
            {
               result = 0;
            }
            else
            {
               Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCanDo (canDo=\"%s\")\n", (const char*)ptr);
            }
            // 
         }
         break;

      case audioMasterGetLanguage: // Allowed before VstPluginMain returns
         // plugin queries what language is set for the host
         // x[returns]: see enum VstHostLanguage
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetLanguage\n");
         }
         result = kVstLangEnglish;
         break;

      case audioMasterOpenWindow: // **DEPRECATED**
         // e[ptr]: pointer to window*
         // x[return]: host returns platform specific pointer*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOpenWindow (**deprecated**)\n");
         }
         break;

      case audioMasterCloseWindow: // **DEPRECATED**
         // close window,
         // e[ptr]: platform specific handle
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCloseWindow (**deprecated**)\n");
         }
         break;

      case audioMasterGetDirectory: // Allowed before VstPluginMain returns
         // plugin requests host for plugin directory
         // x[ptr]: string-pointer to directory, FSSpec on MAC
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetDirectory\n");
         }
         break;

      case audioMasterUpdateDisplay:
      {
         // Something has changed, update the host's 'multi-fx' display.
         // x[return]: 1 = supported
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterUpdateDisplay\n");
         }

         if(NULL != plugin)
         {
            plugin->callOnAudioMasterUpdateDisplay();

            TKVST2Window *vw = FindVST2WindowByPlugin(plugin, 1/*bLock*/);
            if(NULL != vw)
            {
               // Some Native Instruments plugins (e.g. Guitar Rig 4) require this or their UI will stop updating after some time
               ////::UpdateWindow(vw->hWnd);
               ::PostThreadMessage(plugin->thread_id, WM_PAINT, 0, 0);
            }
         }
         result = 1;
      }
      break;

      case audioMasterBeginEdit:
         // plugin calls this before a setParameterAutomated with mouse move (one per Mouse Down). It tells the host that if it needs to, it has to record 
         // automation data for this control
         // e[index]: parameter index
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterBeginEdit\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterParamEdit(1/*bBegin*/, index);
         }
         break;

      case audioMasterEndEdit:
         // plugin calls this after a setParameterAutomated (on Mouse Up). It notifies the host that this control is no more moved by the mouse
         // e[index]: parameter index
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterEndEdit\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterParamEdit(0/*bBegin*/, index);
         }
         break;

      case audioMasterOpenFileSelector:
         // plugin requests: open a fileselector window
         // e[ptr]: pointer to VstFileSelect*
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOpenFileSelector\n");
         }
         break;

      case audioMasterCloseFileSelector:
         // plugin requests: close a fileselector operation, must be always called after an openFileSelector !
         // e[ptr]: pointer to VstFileSelect*
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCloseFileSelector\n");
         }
         break;

      case audioMasterEditFile: // **DEPRECATED**
         // open an editor for audio (defined by XML text in <ptr>)
         // e[ptr]: pointer to XML text
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterEditFile (**deprecated**)\n");
         }
         break;

      case audioMasterGetChunkFile: // **DEPRECATED**
         // plugin requests the native path of currently loading bank or project (called from writeChunk)
         // x[ptr]: pointer to char[2048] or (MAC)sizeof(FSSpec)
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetChunkFile (**deprecated**)\n");
         }
         break;

      case audioMasterGetInputSpeakerArrangement: // **DEPRECATED**
         // x[return]: host returns a pointer to VstSpeakerArrangement* structure
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetInputSpeakerArrangement (**deprecated**)\n");
         }
         break;

      default:
         if(lastOpcode != opcode)
         {
            Dopprintf ("[dbg] tkvst2::HostCallback: (opcode %d)\n index = %d, value = %p, ptr = %p, opt = %f\n", opcode, index, FromVstPtr<void> (value), ptr, opt);
         }
         break;
	}

   lastOpcode = opcode;

   // if( (opcode !=7) && (23 != opcode) ) // filter audioMasterGetTime, audioMasterGetProcessLevel
   // {
   //    Dopprintf("xxx end host callback opcode=%d\n", opcode);
   // }

	return result;
}


// (note) dispatcher args: index, value, ptr, opt(float)

VST2Plugin::VST2Plugin(void) {
   ////yac_host->->printf("xxx VST2Plugin::VST2Plugin: this=0x%p\n", this);
   dll_handle = NULL;
   effect     = NULL;

   shell_plugins = NULL;
   num_shell_plugins = 0u;
   current_shell_plugin_idx = -1;

   parent_hwnd = NULL;

   block_size         = 0;
   num_input_buffers  = 0;
   num_output_buffers = 0;
   b_ext_input_buffers = YAC_FALSE;
   b_ext_output_buffers = YAC_FALSE;

   for(sUI i = 0u; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
   {
      input_buffers[i] = NULL;
      output_buffers[i] = NULL;
      collect_input_buffers[i] = NULL;
      collect_output_buffers[i] = NULL;
   }

   // window_handle = NULL;
   thread_handle = NULL;
   thread_id     = 0;

   ::InitializeCriticalSection(&mtx_dispatcher);
   ::InitializeCriticalSection(&mtx_events);

   LazyInitMtxWindows();
#if 0
   LazyInitMtxParamEdit();
#endif

   initEvents();

   ui_window_left   = -1;
   ui_window_top    = -1;
   ui_window_bottom = -1;
   ui_window_right  = -1;

   queued_resize_w = -1;
   queued_resize_h = -1;

   b_debug = YAC_FALSE;

   collect_num_chunks = 0u;
   collect_chunk_idx  = 0u;

   last_tkvst2_song_playing = YAC_FALSE;
   b_report_transport_playing = YAC_TRUE;
}

VST2Plugin::~VST2Plugin() {
   // yac_host->->printf("xxx VST2Plugin::~VST2Plugin: DTOR this=0x%p effect=0x%p\n", this, effect);
   // yac_host->->printf("xxx VST2Plugin::~VST2Plugin: DTOR effect=0x%p\n", effect);
   close();

   ::DeleteCriticalSection(&mtx_dispatcher); 
   ::DeleteCriticalSection(&mtx_events); 

   freeShellPlugins();
}

void VST2Plugin::StaticInit(void) {
   ::InitializeCriticalSection(&mtx_shell_plugin_uid);
   static_shell_plugin_uid = 0u;

   // Create/open databridge host event handle
   databridge_host_ev = ::CreateEvent(NULL/*lpEventAttributes*/,
                                      FALSE/*bManualReset*/,
                                      FALSE/*bInitialState*/,
                                      DATABRIDGE_HOST_EVENT_NAME
                                      );

   if(NULL == databridge_host_ev)
   {
      yac_host->printf("[---] VST2Plugin::StaticInit: failed to create databridge_host_ev !!!\n");
   }
}

void VST2Plugin::StaticExit(void) {
   // Close databridge host event handle
   ::CloseHandle(databridge_host_ev);
   databridge_host_ev = NULL;
}

sBool VST2Plugin::WaitForDatabridgeHostEvent(sUI _timeout) {
   DWORD res = ::WaitForSingleObject(databridge_host_ev, _timeout);
   return (WAIT_OBJECT_0 == res) ? YAC_TRUE : YAC_FALSE;
}

void VST2Plugin::freeShellPlugins(void) {
   if(NULL != shell_plugins)
   {
      delete [] shell_plugins;
      shell_plugins = NULL;
      num_shell_plugins = 0u;
   }
}

void VST2Plugin::initEvents(void) {
   max_events      = 4096;
   max_events_mask = 4095;

   events = (VstEvents*) ::malloc(sizeof(VstEvents) + sizeof(VstMidiEvent*) * max_events);
   memset(events, 0, sizeof(VstEvents) + sizeof(VstMidiEvent*) * max_events);

   midi_events = (VstMidiEvent*) ::malloc(sizeof(VstMidiEvent) * max_events);
   memset(midi_events, 0, sizeof(VstMidiEvent) * max_events);

   sUI i;
   for(i=0; i<max_events; i++)
   {
      events->events[i] = (VstEvent*) &midi_events[i];

      midi_events[i].type            = kVstMidiType;
      midi_events[i].byteSize        = sizeof(VstMidiEvent);
      midi_events[i].deltaFrames     = 0;
      midi_events[i].flags           = kVstMidiEventIsRealtime;
      midi_events[i].noteLength      = 0;
      midi_events[i].noteOffset      = 0;
      midi_events[i].detune          = 0;
      midi_events[i].noteOffVelocity = 0;
      midi_events[i].reserved1       = 0;
      midi_events[i].reserved2       = 0;
   }

   next_event = 0;

   memset(&time_info, 0, sizeof(VstTimeInfo));

   time_info.tempo  = 125.0; // (note) updated during audioMasterGetTime
   time_info.ppqPos = 0.0;
   time_info.flags  = kVstTempoValid | kVstPpqPosValid;  // (note) kVstTransportPlaying is updated when queried
}

// void VST2Plugin::eventTest(void) {
//    printf("xxx VST2Plugin::eventTest\n");

//    static VstEvents events;
//    static VstEvents events2;
//    static VstMidiEvent midi[2];
//    memset(&events,  0, sizeof(VstEvents));
//    memset(&events2, 0, sizeof(VstEvents));

//    // Send two single events
//    //  ==> VSTs do not seem to like this (either pitchbend does not work or output is silent)

//    // // Note-on
//    // events.events[0] = (VstEvent*) & midi[0];
//    // events.numEvents = 1;

//    // midi[0].type     = kVstMidiType;
//    // midi[0].byteSize = sizeof(VstMidiEvent);
//    // midi[0].flags    = kVstMidiEventIsRealtime;
//    // midi[0].midiData[0] = 0x90;
//    // midi[0].midiData[1] = 48;
//    // midi[0].midiData[2] = 127;

//    // effect->dispatcher(effect, effProcessEvents, 0, 0, &events, 0.0f);

//    // // Pitch-bend
//    // events2.events[0] = (VstEvent*) & midi[1];
//    // events2.numEvents = 1;
//    // midi[1].type     = kVstMidiType;
//    // midi[1].byteSize = sizeof(VstMidiEvent);
//    // midi[1].flags    = kVstMidiEventIsRealtime;
//    // midi[1].midiData[0] = 0xE0;
//    // midi[1].midiData[1] = 0x00;
//    // midi[1].midiData[2] = 0x20;

//    // effect->dispatcher(effect, effProcessEvents, 0, 0, &events2, 0.0f);


//    Send two events at once

//    events.numEvents = 2;
//    events.events[0] = (VstEvent*) & midi[0];
//    events.events[1] = (VstEvent*) & midi[1];

//    midi[0].type     = kVstMidiType;
//    midi[0].byteSize = sizeof(VstMidiEvent);
//    midi[0].flags    = kVstMidiEventIsRealtime;
//    midi[0].midiData[0] = 0x90;
//    midi[0].midiData[1] = 48;
//    midi[0].midiData[2] = 127;

//    midi[1].type     = kVstMidiType;
//    midi[1].byteSize = sizeof(VstMidiEvent);
//    midi[1].flags    = kVstMidiEventIsRealtime;
//    midi[1].midiData[0] = 0xE0;
//    midi[1].midiData[1] = 0x00;
//    midi[1].midiData[2] = 0x20;

//    effect->dispatcher(effect, effProcessEvents, 0, 0, &events, 0.0f);
//}



void YAC_VCALL VST2Plugin::yacGetSignalStringList(YAC_String *_sig) {
   // ---- the number represents the encoded argument type identifiers used by this function
   // ---- 1=int, 2=float, 3=object. bits 0+1 represent argument #0, bits 2+3 represent argument #1 and so on
   _sig->visit("onAudioMasterUpdateDisplay:3 onAudioMasterAutomate:39 onAudioMasterIOChanged:3 onWin32KeyEvent:7 onMacOSKeyDown:7 onAudioMasterParamEdit:23");
}

void YAC_VCALL VST2Plugin::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   if(_id < VST2PLUGIN_NUM_SIGNALS)
   {
      tkvst2_audio_master_signal_funs[_id] = _fun;
   }
}

void VST2Plugin::callOnAudioMasterUpdateDisplay(void) {
   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERUPDATEDISPLAY];
   ////yac_host->printf("xxx callOnAudioMasterUpdateDisplay f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[1];
      args[0].initObject(this, 0);
      ////yac_host->yacMutexFindByName("__DEBUG");
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 1, args);
   }
}

void VST2Plugin::callOnAudioMasterAutomate(sSI _paramIdx, sF32 _value) {

   // yac_host->printf("xxx VST2Plugin::callOnAudioMasterAutomate: call setParameter: idx=%d val=%f\n", _paramIdx, _value);
#if 0
   // (note) [11Jan2022] breaks OPS7 (v1.006) patch loading (plugin sends stale parameter value to host during effSetChunk).
   //                    however: why echo the parameter change back to the plugin in the first place ?
   beginDispatch();
   effect->setParameter(effect, _paramIdx, _value);
   endDispatch();
#endif

   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERAUTOMATE];

   ////yac_host->->printf("xxx callOnAudioMasterAutomate: f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt   (_paramIdx);
      args[2].initFloat (_value);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 3, args);
      ////yac_host->->printf("xxx callOnAudioMasterAutomate: f call returned\n");
   }
}

void VST2Plugin::callOnAudioMasterIOChanged(void) {
#if 0
   // problematic with Softube Tape (calls IOChanged during effSetBlockSize)
   beginEvents(); // to make sure that processReplacing() is not running concurrently
   reallocateIOBuffers();
   endEvents();
#endif

   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERIOCHANGED];

   ////yac_host->->printf("xxx callOnAudioMasterIOChanged: f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[1];
      args[0].initObject(this, 0);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 1, args);
      ////yac_host->->printf("xxx callOnAudioMasterIOChanged: f call returned\n");
   }
}

void VST2Plugin::callOnWin32KeyEvent(sUI _lparam) {
   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONWIN32KEYEVENT];

   // yac_host->->printf("xxx callOnWin32KeyEvent: f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[2];
      args[0].initObject(this, 0);
      args[1].initInt((sSI)_lparam);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 2, args);
      ////yac_host->->printf("xxx callOnWin32KeyEvent: f call returned\n");
   }
}

void VST2Plugin::callOnAudioMasterParamEdit(sSI _bBegin, sSI _paramIdx) {
   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERPARAMEDIT];
   ////yac_host->printf("xxx callOnAudioMasterParamEdit f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt(_bBegin);
      args[2].initInt(_paramIdx);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 3, args);
   }
}


sBool VST2Plugin::loadPlugin(YAC_String *_pathName, sUI _selectShellPluginUID) {

   effect_vendor[0] = 0;
   effect_name[0] = 0;
   current_shell_plugin_idx = -1;

   freeShellPlugins();

   b_debug_paramproperties_once = true;

   shell_plugin_uid = _selectShellPluginUID;

   if(YAC_Is_String(_pathName))
   {
      yac_host->printf("[dbg] VST2Plugin:loadPlugin(\"%s\", shellUID=0x%08x)\n", (const char*)_pathName->chars, _selectShellPluginUID);

      HINSTANCE dllHandle = ::LoadLibrary((char*)_pathName->chars);

      dll_handle = dllHandle;

      if(NULL != dll_handle)
      {
         PluginEntryProc mainProc = (PluginEntryProc) ::GetProcAddress((HMODULE)dll_handle, "VSTPluginMain");

         if(NULL == mainProc)
         {
            mainProc = (PluginEntryProc) ::GetProcAddress((HMODULE)dll_handle, "main");
         }

         if(NULL != mainProc)
         {
#ifdef EARLY_WINDOW_CREATE
            yac_host->printf("[dbg] VST2Plugin::loadPlugin: PRE EARLY_WINDOW_CREATE\n");
            CreateVST2Window(this, 1/*bAddWindow*/, 0/*bShowWindow*/);
            yac_host->printf("[dbg] VST2Plugin::loadPlugin: POST EARLY_WINDOW_CREATE\n");
#endif // EARLY_WINDOW_CREATE

            yac_host->printf("[dbg] VST2Plugin::loadPlugin: creating effect. mainProc=0x%p\n", mainProc);

            if(0u != _selectShellPluginUID)
            {
               ::EnterCriticalSection(&mtx_shell_plugin_uid);
               static_shell_plugin_uid = _selectShellPluginUID;
            }

            effect = mainProc(HostCallback);

            yac_host->printf("[dbg] VST2Plugin::loadPlugin: mainProc() returned. effect=%p\n", effect);

            if(0u != _selectShellPluginUID)
            {
               static_shell_plugin_uid = 0u;
               ::LeaveCriticalSection(&mtx_shell_plugin_uid);
            }

            if(NULL != effect)
            {
               yac_host->printf("[...] VST2Plugin::loadPlugin: OK, plugin openend and effect instance created.\n");

               effect->resvd1 = (VstIntPtr) this;
               // // effect->resvd2 = (VstIntPtr) _selectShellPluginUID;
               effect->resvd2 = NULL;

               yac_host->printf("[dbg] VST2Plugin::loadPlugin: call effGetVendorString\n");
               beginDispatch();
               effect->dispatcher(effect, effGetVendorString, 0, 0, (void*)effect_vendor, 0.0f);
               endDispatch();
               effect_vendor[kVstMaxVendorStrLen-1] = 0;
               yac_host->printf("[dbg] VST2Plugin::loadPlugin: effGetVendorString returned (\"%s\")\n", effect_vendor);

               sSI category = getCategory();

               if(kPlugCategShell == category)
               {
                  sUI shellPluginIdx = 0;
                  yac_host->printf("[dbg] VST2Plugin::loadPlugin: ** is shell plugin **\n");
                  shell_plugins = new vst2_shell_plugin_t[VST2PLUGIN_MAX_SHELL_PLUGINS];
                  memset((void*)shell_plugins, 0, sizeof(vst2_shell_plugin_t) * VST2PLUGIN_MAX_SHELL_PLUGINS);
                  for(;;)
                  {
                     vst2_shell_plugin_t *shellPlugin = &shell_plugins[shellPluginIdx];

                     shellPlugin->effect_name[0] = 0;
                     beginDispatch();
                     sUI shellPluginUID = (sUI)effect->dispatcher(effect, effShellGetNextPlugin, 0, 0, (void*)shellPlugin->effect_name, 0.0f);
                     endDispatch();
                     shellPlugin->uid = shellPluginUID;

                     if(b_debug && (0u == _selectShellPluginUID))
                     {
                        yac_host->printf("[dbg] VST2Plugin::loadPlugin: shell plugin [%u] name=\"%s\" UID=0x%08x\n", shellPluginIdx, shellPlugin->effect_name, shellPlugin->uid);
                     }

                     if((0 == shellPluginUID) || (0 == shellPlugin->effect_name[0]))
                        break;

                     // // if((0u != _selectShellPluginUID) && (shellPluginUID == _selectShellPluginUID))
                     // // {
                     // //    current_shell_plugin_idx = (sSI)shellPluginIdx;
                     // //    break;
                     // // }

                     // Next sub-plugin
                     shellPluginIdx++;
                  }
                  num_shell_plugins = shellPluginIdx;
                  // // if(0u != _selectShellPluginUID)
                  // // {
                  // //    yac_host->printf("[dbg] VST2Plugin::loadPlugin: select sub-plugin UID=0x%08x in shell vendor=\"%s\"\n", _selectShellPluginUID, effect_vendor);
                  // // }
                  // // else
                  // // {
                     yac_host->printf("[dbg] VST2Plugin::loadPlugin: found %u plugins in shell vendor=\"%s\"\n", num_shell_plugins, effect_vendor);
                  // // }
               }
               else
               {
                  yac_host->printf("[dbg] VST2Plugin::loadPlugin: call effGetEffectName\n");
                  beginDispatch();
                  effect->dispatcher(effect, effGetEffectName, 0, 0, (void*)effect_name, 0.0f);
                  endDispatch();
                  effect_name[kVstMaxEffectNameLen-1] = 0;
                  yac_host->printf("[dbg] VST2Plugin::loadPlugin: effGetEffectName returned (\"%s\")\n", effect_name);
               }

               // // if((0u != _selectShellPluginUID) && (-1 == current_shell_plugin_idx))
               // // {
               // //    yac_host->printf("[---] VST2Plugin::loadPlugin: failed to select shell plugin UID=0x%08x, vendor=\"%s\".\n", _selectShellPluginUID, effect_vendor);
               // //    // Failed
               // //    return YAC_FALSE;
               // // }
               // // else
               // // {
                  // Succeeded
                  return YAC_TRUE;
               // // }
            }

            yac_host->printf("[---] VST2Plugin::loadPlugin: failed to create effect instance! (pathName=\"%s\")\n", _pathName->chars);
         }
         else
         {
            yac_host->printf("[---] VST2Plugin::loadPlugin: mainProc is NULL (pathName=\"%s\").\n", (const char*)_pathName->chars);
         }
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::loadPlugin: LoadLibrary(\"%s\") failed. GetLastError=%d\n", _pathName->chars, ::GetLastError());
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::loadPlugin: pathName argument is not a String.\n");
   }

   closeLibrary();

   // Failed
   return YAC_FALSE;
}

void VST2Plugin::open(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effOpen, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::freeIOBuffers(void) {

   // Inputs
   if(num_input_buffers > 0u)
   {
      if(YAC_FALSE == b_ext_input_buffers)
      {
         yac_host->printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d input buffers\n", num_input_buffers);

         sUI i = 0;
         for(i=0; i<num_input_buffers; i++)
         {
            delete [] input_buffers[i];
            input_buffers[i] = NULL;
         }
      }

      num_input_buffers = 0;
   }

   // Outputs
   if(num_output_buffers > 0u)
   {
      if(YAC_FALSE == b_ext_output_buffers)
      {
         yac_host->printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d output buffers\n", num_output_buffers);

         sUI i = 0;
         for(i=0; i<num_output_buffers; i++)
         {
            delete [] output_buffers[i];
            output_buffers[i] = NULL;
         }
      }
      num_output_buffers = 0;
   }

   freeIOBuffersCollect();
}

void VST2Plugin::closeEffect(void) {
   // Shut down effect
   if(NULL != effect)
   {
      yac_host->printf("[dbg] VST2Plugin::close: closing effect..\n");
      beginDispatch();
      effect->dispatcher(effect, effClose, 0, 0, NULL, 0.0f);
      endDispatch();
      yac_host->printf("[...] VST2Plugin::close: OK, effect closed.\n");
      effect = NULL;
   }
}

void VST2Plugin::closeLibrary(void) {
   // Close library
   if(NULL != dll_handle)
   {
      yac_host->printf("[dbg] VST2Plugin::close: closing library..\n");

      ::FreeLibrary(dll_handle);

      dll_handle = NULL;

      yac_host->printf("[...] VST2Plugin::close: OK, library closed..\n");
   }
}

void VST2Plugin::freeEvents(void) {

   if(NULL != events)
   {
      ::free(events);
      events = NULL;
   }

   if(NULL != midi_events)
   {
      ::free(midi_events);
      midi_events = NULL;
   }

   max_events = 0;
   next_event = 0;
}

void VST2Plugin::close(void) {

   closeEditor();
   closeEffect();
   closeLibrary();
   freeIOBuffers();
   freeEvents();

   b_ext_input_buffers = YAC_FALSE;
   b_ext_output_buffers = YAC_FALSE;
}

sSI VST2Plugin::getCategory(void) {
   if(NULL != effect)
   {
      sSI i;

      if(0 == effect->numInputs)
      {
         // Fix Waves CODEX category
         i = VST2PLUGIN_CATEGORY_SYNTH;
      }
      else
      {
         beginDispatch();
         VstIntPtr ip = effect->dispatcher(effect, effGetPlugCategory, 0, 0, NULL, 0.0f);
         i = sSI(ip);
         endDispatch();
      }

      return i;
   }
   return kPlugCategUnknown;
}

sBool VST2Plugin::isEffect(void) {
#if 0
   sSI cat = getCategory();
   sBool r;
   if(kPlugCategUnknown != cat)
   {
      r = ! ((kPlugCategSynth == cat) || (kPlugCategGenerator == cat));
   }
   else
   {
      r = YAC_FALSE;
   }
#else
   sBool r = (0 == (effect->flags & effFlagsIsSynth));
#endif
   return r;
}

sBool VST2Plugin::isInstrument(void) {
#if 0
   sSI cat = getCategory();
   sBool r;
   if(kPlugCategUnknown != cat)
   {
      r = ((kPlugCategSynth == cat) || (kPlugCategGenerator == cat));
   }
   else
   {
      r = YAC_FALSE;
   }
#else
   sBool r = (0 != (effect->flags & effFlagsIsSynth));
#endif
   return r;
}

void VST2Plugin::suspend(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effMainsChanged, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::resume(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effMainsChanged, 0, 1, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::startProcess(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effStartProcess, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::stopProcess(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effStopProcess, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::setSampleRate(sF32 _rate) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, _rate);
      endDispatch();

      time_info.sampleRate = _rate;
   }
}

void VST2Plugin::setBlockSize(sUI _blkSize) {
   // printf("xxx VST2Plugin::setBlockSize(blkSize=%u) effect=%p\n", _blkSize, effect);
   if(NULL != effect)
   {
      // Inform plugin (MUST be suspended!)
      beginDispatch();
      effect->dispatcher(effect, effSetBlockSize,  0, (sSI)_blkSize, NULL, 0.0f);
      endDispatch();

      block_size = _blkSize;
      collect_num_chunks = 0u;

      reallocateIOBuffers();
   }
   // printf("xxx VST2Plugin::setBlockSize: END block_size=%u _blkSize=%u\n", block_size, _blkSize);
}

void VST2Plugin::setBlockSizeCollect(sUI _blockSize, sUI _num) {

   if(NULL != effect)
   {
      beginEvents();  // sync with processReplacing()

      // Inform plugin (MUST be suspended!)
      beginDispatch();
      effect->dispatcher(effect, effSetBlockSize,  0, (sSI)block_size * _num, NULL, 0.0f);
      endDispatch();

      block_size = _blockSize;
      collect_num_chunks = _num;
      collect_chunk_idx = 0u;

      reallocateIOBuffers();

      endEvents();
   }
}

sBool VST2Plugin::setExtInputBuffer(sUI _idx, YAC_Object *_fa) {
   sBool r = YAC_FALSE;

   if(_idx < VST2PLUGIN_MAX_IO_BUFFERS)
   {
      if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         input_buffers[_idx] = (sF32*)fa->yacArrayGetPointer();
         b_ext_input_buffers = YAC_TRUE;
      }
      else
      {
         input_buffers[_idx] = NULL;
      }

      r = true;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::setExtInputBuffer: idx=%u >= %u\n", _idx, VST2PLUGIN_MAX_IO_BUFFERS);
   }

   return r;
}

sBool VST2Plugin::setExtOutputBuffer(sUI _idx, YAC_Object *_fa) {
   sBool r = YAC_FALSE;

   if(_idx < VST2PLUGIN_MAX_IO_BUFFERS)
   {
      if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         output_buffers[_idx] = (sF32*)fa->yacArrayGetPointer();
         b_ext_output_buffers = YAC_TRUE;
      }
      else
      {
         output_buffers[_idx] = NULL;
      }

      r = true;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::setExtOutputBuffer: idx=%u >= %u\n", _idx, VST2PLUGIN_MAX_IO_BUFFERS);
   }

   return r;
}

void VST2Plugin::reallocateIOBuffers(void) {
   // called by setBlockSize and audioMasterIOChanged host opcode dispatcher
   
   // Free old I/O buffers 
   freeIOBuffers();

   // (Re-) allocate sample buffers
   if(block_size > 0)
   {
      num_input_buffers = effect->numInputs;

      if(YAC_FALSE == b_ext_input_buffers)
      {
         // Allocate input buffers
         yac_host->printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d input buffers (%d frames)\n", num_input_buffers, block_size);
         if(num_input_buffers > 0)
         {
            sUI i;
            for(i=0; i<num_input_buffers; i++)
            {
               input_buffers[i] = new sF32[block_size];
               if(NULL != input_buffers[i])
               {
                  memset(input_buffers[i], 0, sizeof(sF32) * block_size);
               }
               else
               {
                  yac_host->printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate input buffer #%u (%u samples)\n", i, block_size);
               }
            }
            for(;i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
            {
               input_buffers[i] = NULL;
            }
         }
      }

      num_output_buffers = effect->numOutputs;

      if(YAC_FALSE == b_ext_output_buffers)
      {
         // Allocate output buffers
         yac_host->printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d output buffers (%d frames)\n", num_output_buffers, block_size);
         if(num_output_buffers > 0)
         {
            sUI i;
            for(i=0; i<num_output_buffers; i++)
            {
               output_buffers[i] = new sF32[block_size];
               if(NULL != output_buffers[i])
               {
                  memset(output_buffers[i], 0, sizeof(sF32) * block_size);
               }
               else
               {
                  yac_host->printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate output buffer #%u (%u samples)\n", i, block_size);
               }
            }
            for(;i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
            {
               output_buffers[i] = NULL;
            }
         }
      }

      reallocateIOBuffersCollect();
   }
   else
   {
      yac_host->printf("[~~~] VST2Plugin::reallocateIOBuffers: _blkSize is 0 ?!!\n");
   }
}

void VST2Plugin::reallocateIOBuffersCollect(void) {

   freeIOBuffersCollect();

   if(block_size > 0)
   {
      if(collect_num_chunks > 1u)
      {
         for(sUI i = 0u; i < num_input_buffers; i++)
         {
            collect_input_buffers[i] = new sF32[block_size * collect_num_chunks * 2];
            memset(collect_input_buffers[i], 0, sizeof(sF32) * block_size * collect_num_chunks * 2);
         }
         
         for(sUI i = 0u; i < num_output_buffers; i++)
         {
            collect_output_buffers[i] = new sF32[block_size * collect_num_chunks * 2];
            memset(collect_output_buffers[i], 0, sizeof(sF32) * block_size * collect_num_chunks * 2);
         }
      }
   }
}

void VST2Plugin::freeIOBuffersCollect(void) {
   for(sUI i = 0u; i < num_input_buffers; i++)
   {
      if(NULL != collect_input_buffers[i])
      {
         delete [] collect_input_buffers[i];
         collect_input_buffers[i] = NULL;
      }
   }

   for(sUI i = 0u; i < num_output_buffers; i++)
   {
      if(NULL != collect_output_buffers[i])
      {
         delete [] collect_output_buffers[i];
         collect_output_buffers[i] = NULL;
      }
   }
}

void VST2Plugin::dispatchGetString(sSI _opcode, sSI _idx, sSI _maxLen, YAC_Value *_r) {
   if(NULL != effect)
   {
      char buf[VST2PLUGIN_MAX_STRING_LEN]; // because of buggy plugins that cause buffer overflows
      memset(buf, 0, VST2PLUGIN_MAX_STRING_LEN);

      YAC_String *s = YAC_New_String();
      s->alloc(_maxLen + 1);

      beginDispatch();
      effect->dispatcher(effect, _opcode, _idx, 0, (void*)buf, 0.0f);
      endDispatch();

      sUI i;
      for(i=0; i<((sUI)_maxLen); i++)
      {
         s->chars[i] = buf[i];
      }
      s->chars[i] = 0;
      s->fixLength();

#ifdef PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW
      i = _maxLen;
      for(; i<VST2PLUGIN_MAX_STRING_LEN; i++)
      {
         if(buf[i])
         {
            yac_host->printf("[~~~] VST2Plugin::dispatchGetString: plugin tries to cause buffer overflow!! (writeIdx %u > maxLen %d) (opcode=%d idx=%d maxLen=%d)\n", i, _maxLen, _opcode, _idx, _maxLen);
            break;
         }
      }
#endif // PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW 
      
      _r->initString(s, 1);
   }
}

sSI VST2Plugin::dispatchGetInt(sSI _opcode) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, _opcode, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::dispatchGetInt: effect is NULL.\n");
      return 0;
   }
}

void VST2Plugin::getEffectName(YAC_Value *_r) {
   dispatchGetString(effGetEffectName, 0, kVstMaxEffectNameLen, _r);
}

void VST2Plugin::getVendorString(YAC_Value *_r) {
   dispatchGetString(effGetVendorString, 0, kVstMaxVendorStrLen, _r);
}

void VST2Plugin::getProductString(YAC_Value *_r) {
   dispatchGetString(effGetProductString, 0, kVstMaxProductStrLen, _r);
}

sSI VST2Plugin::getVendorVersion(void) {
   return dispatchGetInt(effGetVendorVersion);
}

sSI VST2Plugin::getVersion(void) {
   if(NULL != effect)
   {
      return effect->version;
   }
   return 0;
}

sSI VST2Plugin::getUniqueID(void) {
   if(NULL != effect)
   {
      return effect->uniqueID;
   }
   return 0;
}

sSI VST2Plugin::canDo(YAC_String *_text) {
   if(YAC_Is_String(_text))
   {
      // "sendVstEvents"
      // "sendVstMidiEvent"
      // "sendVstTimeInfo"
      // "receiveVstEvents"
      // "receiveVstMidiEvent"
      // "receiveVstTimeInfo"
      // "offline"
      // "plugAsChannelInsert"
      // "plugAsSend"
      // "mixDryWet"
      // "noRealTime"
      // "multipass"
      // "metapass"
      // "1in1out"
      // "1in2out"
      // "2in1out"
      // "2in2out"
      // "2in4out"
      // "4in2out"
      // "4in4out"
      // "4in8out"
      // "8in4out"
      // "8in8out"
      // "midiProgramNames"
      // "conformsToWindowRules"

      if(NULL != effect)
      {
         sSI i;
         beginDispatch();
         i = (VstInt32) effect->dispatcher(effect, effCanDo, 0, 0, (void*)_text->chars, 0.0f);
         endDispatch();
         return i;
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::canDo: _text argument is not a String.\n");
   }
   return 0;
}

sUI VST2Plugin::getFlags(void) {
   if(NULL != effect)
   {
      return effect->flags;
   }
   return 0;
}

sBool VST2Plugin::isSynth(void) {
   if(NULL != effect)
   {
      return (effFlagsIsSynth == (effect->flags & effFlagsIsSynth));
   }
   return 0;
}

sBool VST2Plugin::hasEditor(void) {
   if(NULL != effect)
   {
      return (effFlagsHasEditor == (effect->flags & effFlagsHasEditor));
   }
   return 0;
}

sBool VST2Plugin::canReplacing(void) {
   if(NULL != effect)
   {
      return (effFlagsCanReplacing == (effect->flags & effFlagsCanReplacing));
   }
   return 0;
}

sBool VST2Plugin::programsAreChunks(void) {
   if(NULL != effect)
   {
      return (effFlagsProgramChunks == (effect->flags & effFlagsProgramChunks));
   }
   return 0;
}

sBool VST2Plugin::noSoundInStop(void) {
   if(NULL != effect)
   {
      return (effFlagsNoSoundInStop == (effect->flags & effFlagsNoSoundInStop));
   }
   return 0;
}

sSI VST2Plugin::getNumMidiInputChannels(void) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetNumMidiInputChannels, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   return 0;
}

sSI VST2Plugin::getNumMidiOutputChannels(void) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetNumMidiOutputChannels, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   return 0;
}

sSI VST2Plugin::getInitialDelay(void) {
   if(NULL != effect)
   {
      return effect->initialDelay;
   }
   return 0;
}

void VST2Plugin::idle(void) {
   if(NULL != effect)
   {
      // **deprecated**
      beginDispatch();
      effect->dispatcher(effect, effIdle, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::queueEvent(sU8 _0, sU8 _1, sU8 _2, sU8 _3) {

   beginEvents();

   if(NULL != effect)
   {
      VstMidiEvent *ev = &midi_events[next_event];

      ev->midiData[0] = (char) _0;
      ev->midiData[1] = (char) _1;
      ev->midiData[2] = (char) _2;
      ev->midiData[3] = (char) _3;

      // yac_host->printf("xxx VST2Plugin::queueEvent: data= 0x%02x 0x%02x 0x%02x 0x%02x\n", 
      //        (sU8) ev->midiData[0], 
      //        (sU8) ev->midiData[1], 
      //        (sU8) ev->midiData[2], 
      //        (sU8) ev->midiData[3]
      //        );

      next_event = (next_event + 1) & max_events_mask;
   }

   endEvents();
}

void VST2Plugin::queueNoteOn(sSI _midiChannel, sSI _note, sSI _velocity) {
   ////yac_host->printf("xxx queueNoteOn note=%d vel=%d\n", _note, _velocity);

   if(_velocity < 0)
      _velocity = 0;
   else if(_velocity > 127)
      _velocity = 127;

   queueEvent((sU8) (0x90 | (_midiChannel & 15)),
              (sU8)(_note & 127),
              (sU8)(_velocity & 127)
              );
}

void VST2Plugin::queueNoteOff(sSI _midiChannel, sSI _note, sSI _velocity) {
   if(_velocity < 0)
      _velocity = 0;
   else if(_velocity > 127)
      _velocity = 127;

   queueEvent((sU8)(0x80 | (_midiChannel & 15)),
              (sU8)(_note & 127),
              (sU8)(_velocity & 127)
              );
}

void VST2Plugin::queueChannelPressure(sSI _midiChannel, sSI _value) {
   if(_value < 0)
      _value = 0;
   else if(_value > 127)
      _value = 127;

   queueEvent((sU8) (0xD0 | (_midiChannel & 15)),
              (sU8) (_value & 127)
              );
}

void VST2Plugin::queueCtlChange(sSI _midiChannel, sSI _ctl, sSI _value) {
   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (_ctl & 127),
              (sU8) (_value & 127)
              );
}

void VST2Plugin::queueBankSelect(sSI _midiChannel, sSI _bankMSB, sSI _bankLSB) {
   queueCtlChange(_midiChannel,    0, _bankMSB);
   queueCtlChange(_midiChannel, 0x20, _bankLSB);
}

void VST2Plugin::queueProgramChange(sSI _midiChannel, sSI _program) {
   queueEvent((sU8) (0xC0 | (_midiChannel & 15)),
              (sU8) (_program & 127)
              );
}

void VST2Plugin::queuePitchbend(sSI _midiChannel, sSI _pitchMSB, sSI _pitchLSB) {
   queueEvent((sU8) (0xE0 | (_midiChannel & 15)),
              (sU8) (_pitchLSB & 127),
              (sU8) (_pitchMSB & 127)
              );
}

void VST2Plugin::queuePitchbendf(sSI _midiChannel, sF32 _pitchbend) {
   // _pitchbend must be in -1..1 range

   sU16 val;

   if(_pitchbend >= 0.0f)
   {
      val = 0x2000 + (sU16) (_pitchbend * 8191);
   }
   else
   {
      val = 0x2000 + (sU16) (_pitchbend * 8192);
   }

   queueEvent((sU8) (0xE0 | (_midiChannel & 15)),
              (sU8) (val & 127),
              (sU8) ((val>>7) & 127)
              );
}

void VST2Plugin::queueChannelVolumef(sSI _midiChannel, sF32 _vol) {
   // _vol must be in 0..127 range

   if(_vol >= 1.0f)
   {
      _vol = 1.0f;
   }

   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (0x07),
              (sU8) (_vol * 127)
              );
}

void VST2Plugin::queueChannelPanf(sSI _midiChannel, sF32 _pan) {
   // _pan must be in -1..1 range

   sU8 val;

   if(_pan >= 0.0f)
   {
      val = 64 + ((sU8)(_pan * 63));
   }
   else
   {
      val = 64 + ((sU8)(_pan * 64));
   }

   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (0x08),
              (sU8) val
              );
}

sUI VST2Plugin::queueHostMIDIEventsByFlt(YAC_Object *_hostMIDIEvents,
                                         sSI _fltDev, sSI _fltCh,
                                         sSI _forceCh,
                                         sSI _globalCh,
                                         YAC_Object *_modInputFilterOrNull,
                                         sUI _minMillisec, sUI _maxMillisec
                                         ) {
   sUI r = 0u;

   // // if(YAC_BCHK(_hostMIDIEvents, clid_HostMIDIEvents))  // (note) moved to tkmidipipe (clid_HostMIDIEvents not available here)
   {
      YAC_CAST_ARG(HostMIDIEvents, host, _hostMIDIEvents);
      YAC_CAST_ARG(ModInputFilter, modInputFilter, _modInputFilterOrNull);

      sBool bBeginEv = YAC_FALSE;

      for(sUI i = 0u; i < host->num_midi_events; i++)
      {
         HostMIDIEvent *hev = &host->midi_events[i];

         if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
         {
            if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
            {
               if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
               {
                  if(NULL != effect)
                  {
                     sBool bFilterOk = YAC_TRUE;

                     if(NULL != modInputFilter)
                     {
                        bFilterOk = modInputFilter->checkMessage(hev->midi_data[0],
                                                                 hev->midi_data[1],
                                                                 hev->midi_data[2]
                                                                 );
                     }

                     if(bFilterOk)
                     {
                        if(YAC_FALSE == bBeginEv)
                        {
                           beginEvents();
                           bBeginEv = YAC_TRUE;
                        }

                        // if((_minMillisec > 0) && (_maxMillisec < ~0u))
                        //    printf("xxx VST2Plugin::queueHostMIDIEventsByFlt: min=%u max=%u\n", _minMillisec, _maxMillisec);

                        VstMidiEvent *ev = &midi_events[next_event];

                        if(-1 != _forceCh)
                        {
                           // Used by LPC/MPE mode (single lane VSTi always expects ch0)
                           ev->midiData[0] = (hev->midi_data[0] & 0xF0u) | (sUI(_forceCh) & 15u);
                        }
                        else
                        {
                           ev->midiData[0] = hev->midi_data[0];
                        }

                        ev->midiData[1] = hev->midi_data[1];
                        ev->midiData[2] = hev->midi_data[2];
                        ev->midiData[3] = hev->midi_data[3];

                        // Post range-filter transpose
                        if(NULL != modInputFilter)
                        {
                           if( ((ev->midiData[0] & 0xF0u) == 0x90u) ||
                               ((ev->midiData[0] & 0xF0u) == 0x80u)
                               ) // note on or off ?
                           {
                              sS16 xNote = sS16(ev->midiData[1]) + modInputFilter->note_transpose;
                              ev->midiData[1] = (sU8) sRANGE(xNote, 0, 127);
                           }
                        }

                        next_event = (next_event + 1) & max_events_mask;
                        r++;
                     }
                  }
               }
            }
         }
      }

      if(YAC_TRUE == bBeginEv)
      {
         endEvents();
      }
   }
   
   return r;
}

void VST2Plugin::processEvents(void) {
   if(NULL != effect)
   {
      beginEvents();

      ////yac_host->printf("xxx processEvents: #queued events=%u\n", next_event);
      if(next_event > 0)
      {

         sUI i;
         for(i=0; i<next_event; i++)
         {
            VstMidiEvent *ev = (VstMidiEvent*) events->events[i];
            // yac_host->printf("xxx VST2Plugin::processEvents: event[%u]: 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //        (sU8) ev->midiData[0], 
            //        (sU8) ev->midiData[1], 
            //        (sU8) ev->midiData[2], 
            //        (sU8) ev->midiData[3]
            //        );
         }

         events->numEvents = next_event;

         // // beginDispatch();
         effect->dispatcher(effect, effProcessEvents, 0, 0, events, 0.0f);
         // // endDispatch();

         next_event = 0;
      }

      endEvents();
   }
}

sSI VST2Plugin::getNumInputs(void) {
   if(NULL != effect)
   {
      return effect->numInputs;
   }
   return 0;
}

void VST2Plugin::getInputProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numInputs))
      {
         VST2PinProperties *pin = YAC_NEW(VST2PinProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetInputProperties, _idx, 0, (void*) &pin->properties, 0.0f))
         {
            _r->initObject(pin, 1);
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getInputProperties: feature not supported by plugin.\n");

            // Generate a pin (old-style plugins)
            if(0 == _idx)
            {
               pin->initFallbackPin((2 == effect->numInputs));
               _r->initObject(pin, 1);
            }
            else
            {
               YAC_DELETE(pin);
            }
         }
         endDispatch();

      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getInputProperties: _idx out of range (%u >= %u)\n", _idx, effect->numInputs);
      }
   }
}

sSI VST2Plugin::getNumOutputs(void) {
   if(NULL != effect)
   {
      return effect->numOutputs;
   }
   return 1;
}

void VST2Plugin::getOutputProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numOutputs))
      {
         VST2PinProperties *pin = YAC_NEW(VST2PinProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetOutputProperties, _idx, 0, (void*) &pin->properties, 0.0f))
         {
            _r->initObject(pin, 1);
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getOutputProperties: feature not supported by plugin.\n");

            // Generate a pin (old-style plugins)
            if(0 == _idx)
            {
               pin->initFallbackPin((2 == effect->numOutputs));
               _r->initObject(pin, 1);
            }
            else
            {
               YAC_DELETE(pin);
            }
         }
         endDispatch();
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getOutputProperties: _idx out of range (%u >= %u)\n", _idx, effect->numOutputs);
      }
   }
}

sSI VST2Plugin::getNumPrograms(void) {
   if(NULL != effect)
   {
      return effect->numPrograms;
   }
   return 0;
}

void VST2Plugin::getProgramName(YAC_Value *_r) {
   dispatchGetString(effGetProgramName, 0, kVstMaxProgNameLen, _r);
}

void VST2Plugin::setProgramName(YAC_String *_s) {
   if(YAC_Is_String(_s))
   {
      if(NULL != _s->chars)
      {
         beginDispatch();
         effect->dispatcher(effect, effSetProgramName, 0, 0, (void*)_s->chars, 0.0f);
         endDispatch();
      }
   }
}

void VST2Plugin::getProgramNameIndexed(sSI _idx, YAC_Value *_r) {
   dispatchGetString(effGetProgramNameIndexed, _idx, kVstMaxProgNameLen, _r);
}

void VST2Plugin::setProgram(sSI _program) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effSetProgram, 0, _program, NULL, 0.0f);
      endDispatch();
   }
}

sSI VST2Plugin::getProgram(void) {
   sSI r = 0;
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetProgram, 0, 0, NULL, 0.0f);
      r = sSI(ip);
      endDispatch();
   }
   return r;
}

sSI VST2Plugin::getNumParameters(void) {
   if(NULL != effect)
   {
      return effect->numParams;
   }
   return 0;
}

void VST2Plugin::setParameter(sSI _index, sF32 _value) {
   if(NULL != effect)
   {
      if(NULL != effect->setParameter)
      {
         // yac_host->printf("xxx VST2Plugin::setParameter: idx=%d val=%f\n", _index, _value);
         effect->setParameter(effect, _index, _value);
      }
      else
      {
         yac_host->printf("[~~~] VST2Plugin::setParameter: AEffectSetParameterProc is NULL\n");
      }
   }
}

sF32 VST2Plugin::getParameter(sSI _index) {
   if(NULL != effect)
   {
      if(NULL != effect->getParameter)
      {
         return effect->getParameter(effect, _index);
      }
      else
      {
         yac_host->printf("[~~~] VST2Plugin::getParameter: AEffectGetParameterProc is NULL\n");
      }
   }
   return 0.0f;
}

void VST2Plugin::getParameterName(sSI _index, YAC_Value *_r) {
   dispatchGetString(effGetParamName, _index, 31/*kVstMaxParamStrLen*/, _r); // note: spec says 8 chars max, reality is 31
}

void VST2Plugin::getParameterLabel(sSI _index, YAC_Value *_r) {
   dispatchGetString(effGetParamLabel, _index, kVstMaxParamStrLen, _r);
}

void VST2Plugin::getParameterProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numParams))
      {
         VST2ParameterProperties *param = YAC_NEW(VST2ParameterProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetParameterProperties, _idx, 0, (void*) &param->properties, 0.0f))
         {
            _r->initObject(param, 1);
         }
         else
         {
            if(b_debug && b_debug_paramproperties_once)
            {
               b_debug_paramproperties_once = YAC_FALSE;
               yac_host->printf("[~~~] VST2Plugin::getParameterProperties: feature not supported by plugin (uid=0x%08x).\n", effect->uniqueID);
            }
            YAC_DELETE(param);
            _r->initNull();
         }
         endDispatch();
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getParameterProperties: _idx out of range (%u >= %u)\n", _idx, effect->numParams);
      }
   }
}


static float xxx_a = 0.0f;
static float xxx_w = 440.0f / 44100.0f;

void VST2Plugin::processReplacing(sUI _numFrames, sUI _off) {
   if(NULL != effect)
   {
      // yac_host->printf("VST2Plugin::processReplacing: ENTER, numFrames=%u off=%u\n", _numFrames, _off);
      beginEvents();
      // yac_host->printf("VST2Plugin::processReplacing: 2, numFrames=%u off=%u\n", _numFrames, _off);

#if 0
      if(!b_ext_output_buffers)
         yac_host->printf("\n\n!!!!!!!!! ERRROR processReplacing (b_ext_output_buffers=0) !!!!!!!!!!!!!!!!\n\n");
#endif

      if(_numFrames > block_size)
      {
         yac_host->printf("[~~~] VST2Plugin::processReplacing: _numFrames exceeds block_size (%u > %u). clipping..\n", _numFrames, block_size);
         _numFrames = block_size;
      }
      ////yac_host->printf("xxx process output_buffers[0]=0x%p [1]=0x%p\n", output_buffers[0], output_buffers[1]);
     
      sBool bCanReplacing = (effFlagsCanReplacing == (effect->flags & effFlagsCanReplacing));
      // yac_host->printf("VST2Plugin::processReplacing: 3, bCanReplacing=%d\n", bCanReplacing);

      sF32 *inputBuffers[VST2PLUGIN_MAX_IO_BUFFERS];
      sF32 *outputBuffers[VST2PLUGIN_MAX_IO_BUFFERS];

      sUI i;
      for(i = 0u; i < num_input_buffers; i++)
      {
         inputBuffers[i] = input_buffers[i] + _off;
         // yac_host->printf("xxx VST inputBuffers[%u]=%p\n", i, inputBuffers[i]);
      }
      for(; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
         inputBuffers[i] = NULL;

      for(i = 0u; i < num_output_buffers; i++)
      {
         outputBuffers[i] = output_buffers[i] + _off;
         // yac_host->printf("xxx VST outputBuffers[%u]=%p\n", i, outputBuffers[i]);
      }
      for(; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
         outputBuffers[i] = NULL;

      // yac_host->printf("VST2Plugin::processReplacing: 4 (%u inputBuffers + %u outputBuffers set)\n", num_input_buffers, num_output_buffers);

      if(bCanReplacing)
      {
#ifdef PARANOIA_CHECK_REPLACING_OUTPUT
      // Fill output buffers with NaN
         sUI i;
         for(i=0; i<num_output_buffers; i++)
         {
            memset(outputBuffers[i], 0xcd, sizeof(sF32) * _numFrames);  // -431602080.0f
         }
#endif // PARANOIA_CHECK_REPLACING_OUTPUT
      }
      else
      {
         // Fill output buffers with 0
         sUI i;
         for(i=0; i<num_output_buffers; i++)
         {
            memset(outputBuffers[i], 0, sizeof(sF32) * _numFrames);
         }
      }

      if(bCanReplacing)
      {
         // Replace output buffer samples
         if(NULL != effect->processReplacing)
         {
            if(collect_num_chunks > 1u)
            {
               // Buffered mode
               //  (note) workaround for Zynaptic plugins (e.g. PITCHMAP)
               //  (note) both Reason and Eureka process VST plugins in 64 sample frame chunks
               if(block_size == _numFrames)
               {
                  sF32 *inputBuffersCollect[VST2PLUGIN_MAX_IO_BUFFERS];
                  sF32 *outputBuffersCollect[VST2PLUGIN_MAX_IO_BUFFERS];

                  if(0u == (collect_chunk_idx % collect_num_chunks))
                  {
                     for(i = 0u ; i < num_input_buffers; i++)
                     {
                        inputBuffersCollect[i] = collect_input_buffers[i] + (block_size * ((collect_chunk_idx + collect_num_chunks) % (collect_num_chunks*2))) + _off;
                     }

                     for(i = 0u ; i < num_output_buffers; i++)
                     {
                        outputBuffersCollect[i] = collect_output_buffers[i] + (block_size * ((collect_chunk_idx + collect_num_chunks) % (collect_num_chunks*2))) + _off;
                     }

                     // yac_host->printf("xxx VST2Plugin: process %u buffered sample frames. collect_chunk_idx=%u off=%u\n", block_size * collect_num_chunks, collect_chunk_idx, _off);
#if 1
                     effect->processReplacing(effect, inputBuffersCollect, outputBuffersCollect, (VstInt32)block_size * collect_num_chunks);
#else
                     for(i = 0u; i < 2u; i++)
                     {
                        for(sUI k = 0u; k < (block_size * collect_num_chunks); k++)
                        {
                           outputBuffersCollect[i][k] = inputBuffersCollect[i][k];
                        }
                     }
#endif
                  }

                  // Collect current input chunk
                  for(i = 0u ; i < num_input_buffers; i++)
                  {
                     sF32 *d = collect_input_buffers[i] + (block_size * collect_chunk_idx) + _off;

                     for(sUI k = 0u; k < _numFrames; k++)
                     {
                        d[k] = inputBuffers[i][k];
                     }
                  }

                  // Write current output chunk
                  for(i = 0u ; i < num_output_buffers; i++)
                  {
                     sF32 *s = collect_output_buffers[i] + (block_size * collect_chunk_idx) + _off;

                     for(sUI k = 0u; k < _numFrames; k++)
                     {
                        outputBuffers[i][k] = s[k];
                     }
                  }

                  collect_chunk_idx = (collect_chunk_idx + 1u) % (collect_num_chunks * 2);
               }
            }
            else
            {
               // Unbuffered mode
               // yac_host->printf("xxx call effect->processReplacing() @0x%p inBufs=0x%p outBufs=0x%p\n", effect->processReplacing, input_buffers, output_buffers);
               effect->processReplacing(effect, inputBuffers, outputBuffers, (VstInt32)_numFrames);
               // yac_host->printf("xxx OK\n");
            }
         }
         else
         {
            yac_host->printf("[---] VST2Plugin::processReplacing: fxn not implemented by plugin\n");
         }
      }
      else
      {
         // Add to output buffer samples
         if(NULL != effect->process)
         {
            // yac_host->printf("xxx call effect->process() @0x%p inBufs=0x%p outBufs=0x%p\n", effect->process, input_buffers, output_buffers);
            effect->process(effect, inputBuffers, outputBuffers, (VstInt32)_numFrames);
            // yac_host->printf("xxx OK\n");
         }
         else
         {
            yac_host->printf("[---] VST2Plugin::processReplacing: process fallback fxn not implemented by plugin\n");
         }
      }

      if(0)
      {
         if(num_output_buffers >= 2u)
         {
            sUI x=0;
            // yac_host->printf("xxx render sine outputBuffers=(%p; %p)\n", outputBuffers[0], outputBuffers[1]);
            for(; x < _numFrames; x++)
            {
               outputBuffers[0][x] = ::sinf(xxx_a);
               outputBuffers[1][x] = ::sinf(xxx_a);
               xxx_a += xxx_w;
            }
         }
      }

      // // yac_host->printf("xxx output_buffers[0][0]=%f\n", output_buffers[0][0]);
      // // yac_host->printf("xxx output_buffers[1][0]=%f\n", output_buffers[1][0]);
      // yac_host->printf("xxx end call effect->processReplacing(numFrames=%u off=%u)\n", _numFrames, _off);

#ifdef PARANOIA_CHECK_REPLACING_OUTPUT
      if(bCanReplacing && (num_output_buffers > 0))
      {
         if( 0xcdcdcdcdu == *((sUI*)&outputBuffers[0][0]) )
         {
            yac_host->printf("[~~~] VST2Plugin::processReplacing: effect/instrument did not produce any output\n");

            sUI i;
            for(i=0; i<num_output_buffers; i++)
            {
               memset(outputBuffers[i], 0, sizeof(sF32) * _numFrames);
            }
         }
      }
#endif // PARANOIA_CHECK_REPLACING_OUTPUT

      // Debug print output buffer
      if(0)
      {
         if(num_output_buffers >= 2u)
         {
            sUI i = 0;
            yac_host->printf("xxx output_buffers[0]=");
            for(; i < _numFrames; i++)
            {
               yac_host->printf("%f ", outputBuffers[0][i]);
            }
            yac_host->printf("\n");
         }
      }

      endEvents();
   }

}

void VST2Plugin::processReplacingSilence(sUI _numFrames, sUI _off) {
   if(NULL != effect)
   {
      // yac_host->printf("VST2Plugin::processReplacing: ENTER, numFrames=%u off=%u\n", _numFrames, _off);
      // yac_host->printf("VST2Plugin::processReplacing: 2, numFrames=%u off=%u\n", _numFrames, _off);

      if(!b_ext_output_buffers)
         yac_host->printf("\n\n!!!!!!!!! ERRROR processReplacing (b_ext_output_buffers=0) !!!!!!!!!!!!!!!!\n\n");

      if(_numFrames > block_size)
      {
         yac_host->printf("[~~~] VST2Plugin::processReplacing: _numFrames exceeds block_size (%u > %u). clipping..\n", _numFrames, block_size);
         _numFrames = block_size;
      }

      // Fill output buffers with 0
      for(sUI i = 0u; i < num_output_buffers; i++)
      {
         sF32 *outBuf = output_buffers[i] + _off;
         memset(outBuf, 0, sizeof(sF32) * _numFrames);
      }
   }

}

sBool VST2Plugin::getChunk(YAC_Object *_buffer, sBool _bProgram) {
   if(YAC_Is_Buffer(_buffer))
   {
      YAC_Buffer *buffer = (YAC_Buffer*) _buffer;

      if(NULL != effect)
      {
         void *addr = NULL;

         // (note) plugin allocates chunk memory and is responsible for freeing it
         beginDispatch();
         VstIntPtr ip = effect->dispatcher(effect, effGetChunk, _bProgram, 0, (void*)(&addr), 0.0f);
         sUI size = sUI(ip);
         endDispatch();

         if(NULL != addr)
         {
            // Copy to buffer
            if(buffer->yacArrayAlloc(size, 0,0,0))
            {
               ::memcpy(buffer->buffer, addr, size);

               // Succeeded
               return 1;
            }
            else
            {
               yac_host->printf("[---] VST2Plugin::getChunk: failed to allocate buffer (%u bytes).\n", size);
            }
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getChunk: %s\n",
                   (!programsAreChunks()) ? "feature not supported by plugin" : "plugin did not allocate chunk (addr==NULL)");
         }
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::getChunk: _buffer argument is not a Buffer.\n");
   }

   // Failed
   return 0;
}

void VST2Plugin::setChunk(YAC_Object *_buffer, sBool _bProgram) {
   if(YAC_Is_Buffer(_buffer))
   {
      YAC_Buffer *buffer = (YAC_Buffer*) _buffer;

      if(NULL != effect)
      {
         beginDispatch();
         effect->dispatcher(effect, effSetChunk, _bProgram, (VstInt32) buffer->size, (void*)buffer->buffer, 0.0f);
         endDispatch();
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::getChunk: _buffer argument is not a Buffer.\n");
   }
}



LRESULT CALLBACK WndProcedure_Sub(HWND hWnd, UINT Msg,
                                  WPARAM wParam, LPARAM lParam)
{
   Dwmprintf("[dbg] tkvst2::WndProcedure_Sub: hWnd=%p Msg=0x%08x wParam=%p lParam=%p\n", hWnd, Msg, wParam, lParam);

   // Retrieve original window handler
   WNDPROC childWndProc = (WNDPROC) GetPropA(hWnd, TEXT("__eureka_wndproc"));   
   Dwmprintf("[dbg] tksvt::WndProcedure_Sub:   childWndProc=%p\n", childWndProc);

   switch(Msg)
   {
      case WM_DESTROY: ////NCelse if(WM_NCDESTROY == LOWORD(wParam))
      {
         // Unhook WndProcedure and remove props
         // // HWND childHWND = HWND(lParam);
         // // WNDPROC childWndProc = (WNDPROC) GetPropA(childHWND, TEXT("__eureka_wndproc"));
         Dwmprintf("[dbg] tksvst::WndProcedure_Sub: WM_DESTROY childHWND=%p childWndProc=%p\n", hWnd, childWndProc);
         SetWindowLongPtrA(hWnd, GWLP_WNDPROC, LONG_PTR(childWndProc));
         RemovePropA(hWnd, TEXT("__eureka_wndproc"));
         RemovePropA(hWnd, TEXT("__eureka_hwnd"));
      }
      break;

      case WM_KEYDOWN:
      {
         HWND vstHWND = (HWND) GetPropA(hWnd, TEXT("__eureka_hwnd"));   
         Dwmprintf("[dbg] tkvst2::WndProcedure_Sub: received WM_KEYDOWN lParam=0x%08x vstHWND=%p\n", lParam, vstHWND);
         SetWindowLongPtrA(vstHWND, 0/*OFF_bSubClassed*/, LONG_PTR(1));  // don't process again in WndProcedure()
         TKVST2Window *vw = FindVST2WindowByHandle(vstHWND);
         if(NULL != vw)
         {
            if(! ( ((sUI)lParam) & 0x40000000 ) ) // filter keyrepeat
            {
               vw->plugin->callOnWin32KeyEvent((sUI)lParam);
            }
         }
      }
      break;

      case WM_KEYUP:
      {
         Dwmprintf("[dbg] tkvst2::WndProcedure_Sub: received WM_KEYUP lParam=0x%08x\n", lParam);
         HWND vstHWND = (HWND) GetPropA(hWnd, TEXT("__eureka_hwnd"));   
         SetWindowLongPtrA(vstHWND, 0/*OFF_bSubClassed*/, LONG_PTR(1));
         TKVST2Window *vw = FindVST2WindowByHandle(vstHWND);
         if(NULL != vw)
         {
            vw->plugin->callOnWin32KeyEvent((sUI)lParam);
         }
      }
      break;
   }

   // Call original window handler
   return CallWindowProc(childWndProc, hWnd, Msg, wParam, lParam);
}


LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,
                              WPARAM wParam, LPARAM lParam)
{
#ifdef WM_DEBUG
   if(WM_TIMER != Msg)
   {
      fprintf(stderr, "VST2Plugin: WndProcedure message:  ");
      if ( Msg > MAX_WMMSG ) {
         fprintf(stderr, "%d", Msg);
      } else {
         fprintf(stderr, "%s", loc_wmtab[Msg]);
      }
      fprintf(stderr, " -- 0x%X, 0x%X  hWnd=0x%08x\n", wParam, lParam, hWnd);
   }
#endif

   LONG_PTR bSubClassed = GetWindowLongPtrA(hWnd, 0/*OFF_bSubClassed*/);

   switch(Msg)
   {
      case WM_CREATE:
         Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_CREATE hWnd=%p\n", hWnd);
         return DefWindowProc(hWnd, Msg, wParam, lParam);

      case WM_PARENTNOTIFY:
         {
            Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_PARENTNOTIFY hWnd=%p\n", hWnd);
            // // ::SetFocus(hWnd);
            // ::EnableWindow(hWnd, TRUE);
            // ::SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE)&~WS_POPUP) | WS_CHILD);

            if(WM_CREATE == LOWORD(wParam))
            {
               // Hook WndProcedure (for keyboard events) and remember VST HWND and original wndproc
               HWND childHWND = HWND(lParam);
               WNDPROC childWndProc = (WNDPROC)GetWindowLongPtrA(childHWND, GWLP_WNDPROC);
               Dwmprintf("[dbg] tksvt::WndProcedure:  WM_CREATE childHWND=%p childWndProc=%p\n", childHWND, childWndProc);
               SetWindowLongPtrA(childHWND, GWLP_WNDPROC, LONG_PTR(&WndProcedure_Sub));
               SetWindowLongPtrA(hWnd, 0/*OFF_bSubClassed*/, LONG_PTR(0));
               SetProp(childHWND, TEXT("__eureka_hwnd"), HANDLE(hWnd));
               SetProp(childHWND, TEXT("__eureka_wndproc"), HANDLE(childWndProc));
            }
         }

         return DefWindowProc(hWnd, Msg, wParam, lParam);

      case WM_CLOSE:
         Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_CLOSE\n");
         {
            TKVST2Window *vstWin = FindVST2WindowByHandle(hWnd);
            if((NULL != vstWin) && b_hide_vst_window_when_closed && vstWin->b_allow_redirect_close_to_hide)
            {
               ::ShowWindow(hWnd, SW_HIDE);
            }
            else
            {
               // Stop effIdle timer
               KillTimer (hWnd, 1);
            
               // Destroy window and remove it from window list
               RemoveVST2WindowByHandle(hWnd);
            }
         }
         break;////return DefWindowProc(hWnd, Msg, wParam, lParam);

      // If the user wants to close the application
      case WM_DESTROY:
         Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_DESTROY\n");
         // then close it

         PostQuitMessage(WM_QUIT);
         break;

      case WM_TIMER:
      {
         // yac_host->printf("[dbg] tkvst2::WndProcedure: received WM_TIMER\n");
         TKVST2Window *vw = FindVST2WindowByHandle(hWnd);
         if(NULL != vw)
         {
            // // vw->plugin->beginDispatch();
            vw->plugin->effect->dispatcher(vw->plugin->effect, effEditIdle, 0, 0, 0, 0);
            // // vw->plugin->endDispatch();
         }
      }
      break;

      // (note) [05Aug2019] usually handled in WndProcedure_Sub() but some VSTs (e.g. VSVR) don't recv key events in Sub() handler
      case WM_KEYDOWN:
      Dwmprintf("[dbg] tkvst2::WndProcedure: WM_KEYDOWN: bSubClassed=%p\n", bSubClassed);
      if(0 == bSubClassed)
      {
         Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_KEYDOWN lParam=0x%08x\n", lParam);
         TKVST2Window *vw = FindVST2WindowByHandle(hWnd);
         if(NULL != vw)
         {
            if(! ( ((sUI)lParam) & 0x40000000 ) ) // filter keyrepeat
            {
               vw->plugin->callOnWin32KeyEvent((sUI)lParam);
            }
         }
      }
      return DefWindowProc(hWnd, Msg, wParam, lParam);

      case WM_KEYUP:
      if(0 == bSubClassed)
      {
         Dwmprintf("[dbg] tkvst2::WndProcedure: received WM_KEYUP lParam=0x%08x\n", lParam);
         TKVST2Window *vw = FindVST2WindowByHandle(hWnd);
         if(NULL != vw)
         {
            vw->plugin->callOnWin32KeyEvent((sUI)lParam);
         }
      }
      return DefWindowProc(hWnd, Msg, wParam, lParam);

      default:
         // Process the left-over messages
         return DefWindowProc(hWnd, Msg, wParam, lParam);
   }
   // If something was not done, let it go
   return 0;
}

static ATOM TKVST2_RegisterClass(HINSTANCE _dllHandle, const char *_className) {
   WNDCLASSEX WndClsEx;
   ATOM retAtom;

   // Create the application window
   WndClsEx.cbSize        = sizeof(WNDCLASSEX);
   WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
   WndClsEx.lpfnWndProc   = WndProcedure;
   WndClsEx.cbClsExtra    = 0;
   WndClsEx.cbWndExtra    = sizeof(void*);
   WndClsEx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
   WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   WndClsEx.lpszMenuName  = NULL;
   WndClsEx.lpszClassName = _className;
   WndClsEx.hInstance     = _dllHandle;/*hInstance;*/
   WndClsEx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
      
   // Register the application
   retAtom = RegisterClassEx(&WndClsEx);

   if(0 == retAtom)
   {
      yac_host->printf("[---] tkvst: RegisterClassEx() failed. LastError=%d\n", ::GetLastError());
   }

   return retAtom;
}

static void BuildWindowTitle(VST2Plugin *thiz) {
   // Generate window title (also used as class name)
  
   sprintf(init_windowTitle, "%s / %s -- VST editor (thiz=%p)\000", thiz->effect_vendor, thiz->effect_name, thiz);
}

static TKVST2Window *InitVST2Window(VST2Plugin *thiz, HWND hWnd, sBool bAddWindow, sBool bShowWindow) {

   // bPreInit: when called when plugin is opened and effect has not been instanciated, yet

   TKVST2Window *vw = NULL;

   // Start timer (for effIdle calls)
   if(bAddWindow)
   {
      SetTimer(hWnd, 1, 20, 0);
   }


   // // thiz->beginDispatch();
   // // thiz->effect->dispatcher(thiz->effect, effEditIdle, 0, 0, 0, 0);
   // // thiz->endDispatch();

   // Why??? without this loop, some VSTs (e.g. Pianoteq) will freeze the entire app
   if(1)
   {
      MSG msg;
      while(::PeekMessage(&msg, NULL, 0, 0, 1))
      // while(::PeekMessage(&msg, hWnd, 0, 0, 1))
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }

   sSI bOK = 1;

   if(bOK)
   {
      if(bShowWindow)
      {
         yac_host->printf("[dbg] tkvst2::InitVST2Window: 6 call effEditGetRect\n");

         ERect* eRect = 0;
         thiz->beginDispatch();
         thiz->effect->dispatcher (thiz->effect, effEditGetRect, 0, 0, &eRect, 0);
         thiz->endDispatch();

         yac_host->printf("[dbg] tkvst2::InitVST2Window: 6b calleffEditGetRect returned\n");

         if(eRect)
         {
            yac_host->printf("[dbg] tkvst2::InitVST2Window: 7\n");

            sSI width  = eRect->right  - eRect->left;
            sSI height = eRect->bottom - eRect->top;
            if(width < 100)
               width = 100;
            if(height < 100)
               height = 100;

            yac_host->printf("[dbg] tkvst2::InitVST2Window: 8 effEditGetRect indicates size=(%d; %d)\n", width, height);

            if(0x48503438 == thiz->effect->uniqueID) // Tranzistow reports wrong height
            {
               height = 566;
            }
         
            RECT wRect;
            ::SetRect(&wRect, 0, 0, width, height);
            ::AdjustWindowRectEx(&wRect, ::GetWindowLong(hWnd, GWL_STYLE), FALSE, ::GetWindowLong(hWnd, GWL_EXSTYLE));
            width  = wRect.right  - wRect.left;
            height = wRect.bottom - wRect.top;

            yac_host->printf("[dbg] tkvst2::InitVST2Window: size after AdjustWindowRectEx() is (%d; %d).\n", width, height);

#if 0
            if(-1 != thiz->ui_window_left)
            {
               // Have cached window geometry
               yac_host->printf("[dbg] tkvst2::InitVST2Window: using cached window geometry pos=(%d; %d) size=(%d; %d)\n",
                                thiz->ui_window_left, thiz->ui_window_top,
                                (thiz->ui_window_right - thiz->ui_window_left), (thiz->ui_window_bottom - thiz->ui_window_top)
                                );

               ::SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);

               ::MoveWindow(hWnd, 
                            thiz->ui_window_left, 
                            thiz->ui_window_top, 
                            (thiz->ui_window_right - thiz->ui_window_left), 
                            (thiz->ui_window_bottom - thiz->ui_window_top),
                            TRUE /* repaint*/
                            );
            }
            else
#endif
            {
               // Use default position and size
               ::SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
            }
         }
         else
         {
            bOK = 0;
            yac_host->printf("[---] tkvst2::InitVST2Window: effEditGetRect() failed (eRect=NULL)\n");
         }
      } // if !bPreInit

      if(bOK)
      {
         yac_host->printf("[dbg] tkvst2::InitVST2Window: 9\n");

         if(bAddWindow)
         {
            // Remember in window list
            LockWindows();

            vw = new TKVST2Window;
            vw->next   = first_window;
            vw->hWnd   = hWnd;
            vw->plugin = thiz;
            vw->b_allow_redirect_close_to_hide = YAC_TRUE;
            ::strncpy(vw->window_title, init_windowTitle, TKVST2_MAX_WINDOWTITLE_SIZE);
            first_window = vw;

            UnlockWindows();
         }

         yac_host->printf("[dbg] tkvst2::InitVST2Window: 10 call ShowWindow(hWnd=%p)\n", hWnd);

         if(bShowWindow)
         {
            // Display the window to the user
            ::ShowWindow(hWnd, SW_SHOWNORMAL);
            yac_host->printf("[dbg] tkvst2::InitVST2Window: 10b call UpdateWindow(hWnd=%p)\n", hWnd);

            ::UpdateWindow(hWnd);
            yac_host->printf("[dbg] tkvst2::InitVST2Window: 11\n");

            ::SetFocus(hWnd);
            yac_host->printf("[dbg] tkvst2::InitVST2Window: 11b\n");

            // (note) there's exactly _one_ plugin out of the 400+ I've tested that requires effEditOpen
            //         to be called AFTER creating/showing the window, and that's Waldorf's "Nave"
            //         (otherwise it crashes in d2ddevicecontextbase::createbitmapfromwicbitmap())
            if(bShowWindow)
            {
               yac_host->printf("[dbg] tkvst2::InitVST2Window: 5a calling effEditOpen\n");
               thiz->beginDispatch();
               VstIntPtr ip = thiz->effect->dispatcher(thiz->effect, effEditOpen, 0, 0, hWnd, 0.0f);
               bOK = sSI(ip);
               // (note) Softube Heartbeat and Tape plugins return 0 here (all other tested plugins return 1)
               // (note)  I'm not sure if the VST spec expects the return value to matter (it would make sense, though)
               bOK = 1;

#if 0
               thiz->effect->dispatcher(thiz->effect, effEditTop, 0, 0, NULL, 0);   // deprecated in vst2.4
               thiz->effect->dispatcher(thiz->effect, effEditIdle, 0, 0, NULL, 0);  // deprecated in vst2.4
#endif

               thiz->endDispatch();

               // To front
               ::SetActiveWindow(hWnd);

               yac_host->printf("[dbg] tkvst2::InitVST2Window: 5b effEditOpen returned\n");
            }
            else
            {
               bOK = 1;
            }
         }
      }
   }
   else
   {
      yac_host->printf("[---] tkvst2::InitVST2Window: effEditOpen failed (ret=%d)\n", bOK);
   }

   // Succeeded
   return vw;
}

static TKVST2Window *CreateVST2Window(VST2Plugin *thiz, sBool bAddWindow, sBool bShowWindow) {

   TKVST2Window *vw = NULL;

   yac_host->printf("[dbg] tkvst2::CreateVST2Window: ENTER. bAddWindow=%d bShowWindow=%d\n", bAddWindow, bShowWindow);


   // HINSTANCE hInstance = thiz->dll_handle;
   HINSTANCE hInstance = ::GetModuleHandle(NULL);

   yac_host->printf("[dbg] tkvst2::CreateVST2Window: 3\n");

   if(bShowWindow)
   {
      BuildWindowTitle(thiz);
   }
   else
   {
      sprintf(init_windowTitle, "VST plugin=%p", thiz);
   }

   ATOM atom = TKVST2_RegisterClass(hInstance, init_windowTitle/*className*/);

   yac_host->printf("[dbg] tkvst2::CreateVST2Window: 4\n");

   if(0 != atom)
   {
      // Create the window object
      HWND hWnd = ::CreateWindow(init_windowTitle/*lpClassName*/,
                                 init_windowTitle/*lpWindowName*/,
                                 0
                                 // //|WS_OVERLAPPEDWINDOW
                                 // |WS_OVERLAPPED | WS_CAPTION
                                 // |WS_GROUP
                                 // //|WS_DISABLED
                                 // //|WS_ICONIC  // => initial window is too large
                                 |WS_SYSMENU  // add close button
                                 //| WS_EX_NOPARENTNOTIFY 
                                 ,
                                 CW_USEDEFAULT/*x*/,
                                 CW_USEDEFAULT/*y*/,
                                 CW_USEDEFAULT/*nWidth*/,
                                 CW_USEDEFAULT/*nHeight*/,
                                 (HWND)thiz->parent_hwnd/*hWndParent*/,
                                 NULL/*hMenu*/,
                                 hInstance,
                                 NULL/*lpParam*/
                                 );

      yac_host->printf("[dbg] tkvst2::CreateVST2Window 5 hWnd=0x%p\n", hWnd);
	
      if(NULL != hWnd )
      {
         sBool bOK = 1;

         SetWindowLongPtrA(hWnd, 0/*bSubClassed*/, LONG_PTR(0));
         ::SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)&~WS_SIZEBOX);

#ifdef SET_PFD_SUPPORT_OPENGL   // (note) [20Jan2018] added this code while trying to fix support for Waldorf Nave. no luck so far :/

         // pfd init adapted from <http://nehe.gamedev.net/tutorial/creating_an_opengl_window_(win32)/13001/>
         BYTE bits = 32;

         static	PIXELFORMATDESCRIPTOR pfd=	 // <nehe> pfd Tells Windows How We Want Things To Be
            {
               sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
               1,                      // Version Number
               PFD_DRAW_TO_WINDOW |    // Format Must Support Window
               PFD_SUPPORT_OPENGL |    // Format Must Support OpenGL
               PFD_DOUBLEBUFFER,       // Must Support Double Buffering
               PFD_TYPE_RGBA,          // Request An RGBA Format
               bits,                   // Select Our Color Depth
               0, 0, 0, 0, 0, 0,       // Color Bits Ignored
               0,                      // No Alpha Buffer
               0,                      // Shift Bit Ignored
               0,                      // No Accumulation Buffer
               0, 0, 0, 0,             // Accumulation Bits Ignored
               16,                     // 16Bit Z-Buffer (Depth Buffer)
               0,                      // No Stencil Buffer
               0,                      // No Auxiliary Buffer
               PFD_MAIN_PLANE,         // Main Drawing Layer
               0,                      // Reserved
               0, 0, 0                 // Layer Masks Ignored
            };

         // <nehe> If there were no errors while creating the window, we'll attempt to get an OpenGL Device Context. If we can't get a DC an error message will pop onto the screen, and the program will quit (return FALSE).
         HDC hDC = GetDC(hWnd);

         if(NULL != hDC)  // <nehe> Did We Get A Device Context?
         {
            // <nehe> If we managed to get a Device Context for our OpenGL window we'll try to find a pixel format that matches the one we described above. If Windows can't find a matching pixel format, an error message will pop onto the screen and the program will quit (return FALSE).

            sU32 PixelFormat;  // <nehe> Holds The Results After Searching For A Match
            
            if(PixelFormat = ChoosePixelFormat(hDC, &pfd))  // Did Windows Find A Matching Pixel Format?
            {
               // <nehe> If windows found a matching pixel format we'll try setting the pixel format. If the pixel format cannot be set, an error message will pop up on the screen and the program will quit (return FALSE).

               if(SetPixelFormat(hDC, PixelFormat, &pfd))  // <nehe> Are We Able To Set The Pixel Format?
               {
               }
               else
               {
                  yac_host->printf("[---] tkvst2::CreateVST2Window: SetPixelFormat() failed.\n");
                  bOK = 0;
               }
            }
            else
            {
               yac_host->printf("[---] tkvst2::CreateVST2Window: ChoosePixelFormat() failed.\n");
               bOK = 0;
            }
         }
         else
         {
            yac_host->printf("[---] tkvst2::CreateVST2Window: GetDC() failed.\n");
            bOK = 0;
         }
#endif // SET_PFD_SUPPORT_OPENGL

         if(bOK)
         {
            // Open VST editor
            vw = InitVST2Window(thiz, hWnd, bAddWindow, bShowWindow);
         }
      } // if hWnd
      else
      {
         yac_host->printf("[---] tkvst2::CreateVST2Window: CreateWindow() failed (LastError=%d).\n", GetLastError());

         ::UnregisterClass(init_windowTitle, hInstance);
      }
   } // if atom
   else
   {
      yac_host->printf("[---] tkvst2::CreateVST2Window: RegisterClassEx() failed (LastError=%d).\n", GetLastError());
   }

   yac_host->printf("[dbg] tkvst2::CreateVST2Window: LEAVE.\n");
   
   return vw;
}

// // DWORD WINAPI tkvst2__threadentry(LPVOID _VST2Plugin) {
// // // DWORD WINAPI tkvst2__threadentry(LPVOID _VST2Window) {
// //    VST2Plugin *thiz = (VST2Plugin*) _VST2Plugin;
// //    // TKVST2Window *vw = (TKVST2Window*) _VST2Window;
// //    // VST2Plugin *thiz = vw->plugin;

// //    // Signal main thread
// //    ::SetEvent(ev_threadstart); // xxxx

// //    // Open window and inform plugin
// // #ifdef EARLY_WINDOW_CREATE
// //    // Window has already been created when plugin was openend
// //    TKVST2Window *vw = FindVST2WindowByPlugin(thiz, 1/*bLock*/);
// // #else
// //    TKVST2Window *vw = CreateVST2Window(thiz, 1/*bAddWindow*/, 0/*bShowWindow*/);
// // #endif // EARLY_WINDOW_CREATE

// //    // Signal main thread
// //    //  (note) setting the event at this point will cause some plugins (e.g. TAL reverb) to freeze the UI
// //    // // ::SetEvent(ev_threadstart);

// //    if(NULL != vw)
// //    {
// //       MSG msg;

// //       yac_host->printf("[dbg] tkvst: entering UI thread mainloop\n");

// //       // Main event loop
// //       while(::GetMessage(&msg, NULL, 0, 0))
// //       // while(::GetMessage(&msg, vw->hWnd, 0, 0))
// //       {
// //          // yac_host->printf("xxx VST2Plugin: thread mainloop: calling TranslateMessage\n");
// //          ::TranslateMessage(&msg);
// //          // yac_host->printf("xxx VST2Plugin: thread mainloop: calling DispatchMessage\n");
// //          ::DispatchMessage(&msg);
// //          // yac_host->printf("xxx VST2Plugin: thread mainloop: DispatchMessage returned\n");
// //       }

// //       yac_host->printf("[dbg] tkvst: leaving UI thread mainloop\n");
// //    }

// //    return 0;
// // }

void VST2Plugin::setParentNativeWindowHandle(sUI _hWnd) {
   // (note) window handles are always 32bit
   parent_hwnd = (void*)_hWnd;
}

void VST2Plugin::showEditor(void) {
   if(NULL != effect)
   {
      yac_host->printf("[dbg] VST2Plugin::showEditor: uid=0x%08x\n", getUniqueID());

      // Open editor if it is not already open
      TKVST2Window *vw = FindVST2WindowByPlugin(this, 1/*bLock*/);

      if(NULL == vw) 
      {
         // (note) Pianoteq seems to require the VST window (and messageloop) to be located in the main thread, otherwise
         //         effEditOpen() will hang
         yac_host->printf("[dbg] VST2Plugin::showEditor: call CreateVST2Window(this=%p)\n", this);
         TKVST2Window *vw = CreateVST2Window(this, 1/*bAddWindow*/, 1/*bShowWindow*/);
         yac_host->printf("[dbg] VST2Plugin::showEditor: END CreateVST2Window(this=%p)\n", this);
         
         return;
      }
      else
      {
         // // Window already exists (EARLY_CREATE_WINDOW)
         // InitVST2Window(this, vw->hWnd, 0/*bAddWindow*/, 1/*bShowWindow*/);
         // To front
         yac_host->printf("[dbg] VST2Plugin::showEditor: window exists, calling ShowWindow()\n");
         ::ShowWindow(vw->hWnd, SW_SHOWNORMAL);
         ::SetActiveWindow(vw->hWnd);
      }
   } // if effect
}

void VST2Plugin::hideEditor(void) {
   yac_host->printf("[dbg] VST2Plugin::hideEditor: uid=0x%08x\n", getUniqueID());
   TKVST2Window *vw = FindVST2WindowByPlugin(this, false);

   if(NULL != vw)
   {
      ::ShowWindow(vw->hWnd, SW_HIDE);
   }
}

void VST2Plugin::closeEditor(void) {
   sUI uid = getUniqueID();
   if(0u != uid)  // not the template instance ?
   {
      yac_host->printf("[dbg] VST2Plugin::closeEditor: uid=0x%08x\n", uid);
      // LockWindows();
      TKVST2Window *vw = FindVST2WindowByPlugin(this, false);

      if(NULL != vw)
      {
         // UnlockWindows();
         vw->b_allow_redirect_close_to_hide = YAC_FALSE;

         ::PostMessage(vw->hWnd, WM_CLOSE, 0, 0);

         // ::Sleep(2500);

#if 1
         // causes some VST to freeze to UI (e.g. TAL reverb)
         yac_host->printf("[dbg] VST2Plugin::closeEditor: wait until editor window has been closed.\n");
         int iter = 0;
         while(FindVST2WindowByPlugin(this, true))
         {
            yac_host->printf("...\n");

            // Why???? without this loop, some plugins freeze the entire app..
            {
               MSG msg;
               while(::PeekMessage(&msg, NULL, 0, 0, 1))
                  // while(::PeekMessage(&msg, vw->hWnd, 0, 0, 1))
               {
                  ::TranslateMessage(&msg);
                  ::DispatchMessage(&msg);
               }
            }
            if(++iter > 50)
               break;
            ::Sleep(20);
         }
         yac_host->printf("[dbg] VST2Plugin::closeEditor: OK, editor window has been closed.\n");
#endif // 0
      }
      // else
      // {
      //    UnlockWindows();
      // }
   }
}

sBool VST2Plugin::isEditorVisible(void) {
   if(NULL != effect)
   {
      TKVST2Window *vw = FindVST2WindowByPlugin(this, true/*bLock*/);

      if(NULL != vw)
      {
#if 0
         WINDOWPLACEMENT windowPlacement;
         ::GetWindowPlacement(vw->hWnd, &windowPlacement);
         yac_host->printf("[dbg] VST2Plugin::isEditorVisible: windowPlacement.showCmd=%d\n", windowPlacement.showCmd);
         return (SW_HIDE != windowPlacement.showCmd);
#endif

         LONG info = GetWindowLong(vw->hWnd, GWL_STYLE);
         return (0 != (info & WS_VISIBLE));
      }
   }
   return YAC_FALSE;
}

sBool VST2Plugin::isEditorOpen(void) {
   if(NULL != effect)
   {
      TKVST2Window *vw = FindVST2WindowByPlugin(this, true/*bLock*/);
      return (NULL != vw);
   }
   return YAC_FALSE;
}

sUI VST2Plugin::GetNumOpenWindows(void) {
   sUI r = 0;

   LockWindows();

   TKVST2Window *vw = first_window;
   while(NULL != vw)
   {
      r++;
      vw = vw->next;
   }

   UnlockWindows();

   return r;
}

void VST2Plugin::PumpEvents(void) {
   MSG msg;
   
   while(::PeekMessage(&msg, NULL, 0, 0, 1))
   {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);

      // yac_host->printf("[dbg] VST2Plugin::PumpEvents: queued_resize_w=%d\n", queued_resize_w);
   }
}

void VST2Plugin::queueResizeWindow(sUI _w, sUI _h) {

   if( (_w <= 16384u) && (_h <= 16384u) ) {

      yac_host->printf("[dbg] VST2Plugin::queueResizeWindow: queue geo=(%u; %u)\n", _w, _h);
      queued_resize_w = sSI(_w);
      queued_resize_h = sSI(_h);
      queued_window_schedule_ms = yac_host->yacMilliSeconds() + 150u;
   }
   else {
      yac_host->printf("[---] VST2Plugin::queueResizeWindow: insane window size (%u; %u) (ignored)\n", _w, _h);
   }
}

void VST2Plugin::HandleQueuedWindowResize(void) {
   // Handle window resize (audioMasterSizeWindow)
   TKVST2Window *vw = first_window;

   while(NULL != vw)
   {
      if(NULL != vw->plugin)
      {
         if(-1 != vw->plugin->queued_resize_w) {

            if(vw->plugin->queued_window_schedule_ms < yac_host->yacMilliSeconds())
            {
               yac_host->printf("[dbg] VST2Plugin::HandleQueuedWindowResize: vw=%p hWnd=%p queued_resize=(%d; %d)\n", vw, vw->hWnd, vw->plugin->queued_resize_w, vw->plugin->queued_resize_h);

               RECT cr;
               RECT wr;
               POINT border;

               GetClientRect(vw->hWnd, &cr);
               GetWindowRect(vw->hWnd, &wr);

               border.x = (wr.right - wr.left) - cr.right;
               border.y = (wr.bottom - wr.top) - cr.bottom;

               // yac_host->printf("xxx wr.left=%d wr.top=%d cr.right=%d cr.bottom=%d => border=(%d; %d)\n", wr.left, wr.top, cr.right, cr.bottom, border.x, border.y);

#if 1
               // (note) wrong size (title+borders not included)
               ::SetWindowPos(vw->hWnd,
                              NULL/*hWndInsertAfter*/,
                              0/*x*/,
                              0/*y*/,
                              vw->plugin->queued_resize_w + border.x,
                              vw->plugin->queued_resize_h + border.y,
                              (
                                 SWP_NOMOVE        |
                                 SWP_NOOWNERZORDER |
                                 SWP_NOZORDER      |
                                 SWP_SHOWWINDOW    |
                                 SWP_DRAWFRAME     |
                                 0u
                               )
                              );
#else
               MoveWindow(vw->hWnd,
                          wr.left,
                          wr.top,
                          queued_resize_w + border.x,
                          queued_resize_h + border.y,
                          TRUE/*bRepaint*/
                          );

               // ::MoveWindow(vw->hWnd,
               //              0/*x*/,
               //              0/*y*/,
               //              vw->plugin->queued_resize_w,
               //              vw->plugin->queued_resize_h,
               //              TRUE/*bRepaint*/
               //              );
#endif

               vw->plugin->queued_resize_w = -1;
               vw->plugin->queued_resize_h = -1;
            } // if schedule
         } // if queued_resized_w
      } // if vw->plugin

      vw = vw->next;
   } // loop windows
}

void VST2Plugin::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

sUI VST2Plugin::getNumShellPlugins(void) {
   return num_shell_plugins;
}

void VST2Plugin::getShellPluginNameByIndex(sUI _index, YAC_Value *_r) {
   if(_index < num_shell_plugins)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[_index];
      YAC_String *s = YAC_New_String();
      // // s->alloc(VST2PLUGIN_MAX_STRING_LEN);
      s->copy(shellPlugin->effect_name);
      _r->initString(s, YAC_TRUE/*bDelete*/);
   }
   else
   {
      _r->initNull();
   }
}

sUI VST2Plugin::getShellPluginUIDByIndex(sUI _index) {
   sUI r = 0u;

   if(_index < num_shell_plugins)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[_index];
      r = shellPlugin->uid;
   }

   return r;
}

sBool VST2Plugin::selectShellPluginByUID(YAC_String *_name, sUI _uid) {
   // (note) not used, selection is done in loadPlugin()
   sBool r = YAC_FALSE;
   current_shell_plugin_idx = -1;

   for(sUI shellIdx = 0u; shellIdx < num_shell_plugins; shellIdx++)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[shellIdx];

      if(shellPlugin->uid == _uid)
      {
         current_shell_plugin_idx = shellIdx;
         r = YAC_TRUE;
         break;
      }
   }

   return r;
}

sUI VST2Plugin::getCurrentShellPluginUID(void) {
   sUI r = 0u;

   if(-1 != current_shell_plugin_idx)
   {
      if(sUI(current_shell_plugin_idx) < num_shell_plugins)
      {
         const vst2_shell_plugin_t *shellPlugin = &shell_plugins[sUI(current_shell_plugin_idx)];
         r = shellPlugin->uid;
      }
   }

   return r;
}

void VST2Plugin::getCurrentShellPluginEffectName(YAC_Value *_r) {
  _r->initNull();

   if(-1 != current_shell_plugin_idx)
   {
      if(sUI(current_shell_plugin_idx) < num_shell_plugins)
      {
         const vst2_shell_plugin_t *shellPlugin = &shell_plugins[sUI(current_shell_plugin_idx)];
         _r->initEmptyString();
         _r->value.string_val->copy(shellPlugin->effect_name);
      }
   }
}

sUI VST2Plugin::dataBridgeGetNumChunksAvail(void) {
   beginDispatch();
   sUI r = effect->dispatcher(effect, effDataBridgeGetNumChunksAvail, 0, 0, (void*)NULL, 0.0f);
   endDispatch();
   return r;
}
  
void VST2Plugin::SetEnableHideVSTEditorWhenClosed(sBool _bEnable) {
   b_hide_vst_window_when_closed = (YAC_FALSE != _bEnable);
}

sUI VST2Plugin::getNativeWindowHandle(void) {
   TKVST2Window *vw = FindVST2WindowByPlugin(this, YAC_FALSE/*bLock*/);
   if(NULL != vw)
   {
      return sUI(vw->hWnd);
   }
   return 0u;
}

void VST2Plugin::setEnableReportTransportPlaying(sBool _bEnable) {
   b_report_transport_playing = _bEnable;
}

sBool VST2Plugin::getEnableReportTransportPlaying(void) {
   return b_report_transport_playing;
}

void VST2Plugin::windowToFront(void) {
   // not needed on Windows (macOS-only workaround)
}

void VST2Plugin::callEffEditIdle(void) {
   if(isEditorOpen())
   {
      effect->dispatcher(effect, effEditIdle, 0, 0, 0, 0);
   }
}
