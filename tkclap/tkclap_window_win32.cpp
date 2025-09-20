/// tkclap_window_win32.cpp
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 05Jul2024
///
///
///

/// when defined, print wndprocedure debug messages
// #define WM_DEBUG defined

#include "tkclap.h"

void tkclap_window_remove_by_hwnd (HWND _hWnd);

// ---------------------------------------------------------------------------- module vars
static tkclap_mutex_t mtx_windows;
static sBool b_mtx_windows = YAC_FALSE;

static TKCLAPWindow *first_window = NULL;
static char loc_init_window_title[TKCLAP_MAX_WINDOWTITLE_SIZE];

// ---------------------------------------------------------------------------- tkclap_window_lazy_init_mtx_windows
void tkclap_window_lazy_init_mtx_windows(void) {
   if(!b_mtx_windows)
   {
      b_mtx_windows = YAC_TRUE;
      tkclap_mutex_init(&mtx_windows);
   }
}

static inline void tkclap_window_lock(void) {
   tkclap_mutex_lock(&mtx_windows);
}

static inline void tkclap_window_unlock(void) {
   tkclap_mutex_unlock(&mtx_windows);
}

// ---------------------------------------------------------------------------- WM_DEBUG
#ifdef WM_DEBUG
#define Dwmprintf if(0);else yac_host->printf
#else
#define Dwmprintf if(1);else yac_host->printf
#endif // WM_DEBUG

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

// ---------------------------------------------------------------------------- tkclap_window_find_by_hwnd
static TKCLAPWindow *tkclap_window_find_by_hwnd(HWND hWnd) {
   // yac_host->printf("xxx tkclap_window_find_by_hwnd 1\n");
   tkclap_window_lock();
   // yac_host->printf("xxx tkclap_window_find_by_hwnd 2\n");

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->hWnd == hWnd)
         break;
      else
         vw = vw->next;
   }

   // yac_host->printf("xxx tkclap_window_find_by_hwnd 3\n");

   tkclap_window_unlock();
   // yac_host->printf("xxx tkclap_window_find_by_hwnd 4\n");
   return vw;
}

// ---------------------------------------------------------------------------- WndProcedure_Sub
static LRESULT CALLBACK WndProcedure_Sub(HWND hWnd, UINT Msg,
                                         WPARAM wParam, LPARAM lParam
                                         ) {
   Dwmprintf("[dbg] tkclap:WndProcedure_Sub: hWnd=%p Msg=0x%08x wParam=%p lParam=%p\n", hWnd, Msg, wParam, lParam);

   // Retrieve original window handler
   WNDPROC childWndProc = (WNDPROC) GetPropA(hWnd, TEXT("__eureka_wndproc"));
   Dwmprintf("[dbg] tkclap:WndProcedure_Sub:   childWndProc=%p\n", childWndProc);

   switch(Msg)
   {
      case WM_DESTROY: ////NCelse if(WM_NCDESTROY == LOWORD(wParam))
      {
         // Unhook WndProcedure and remove props
         // // HWND childHWND = HWND(lParam);
         // // WNDPROC childWndProc = (WNDPROC) GetPropA(childHWND, TEXT("__eureka_wndproc"));
         Dwmprintf("[dbg] tkclap:WndProcedure_Sub: WM_DESTROY childHWND=%p childWndProc=%p\n", hWnd, childWndProc);
         ::SetWindowLongPtrA(hWnd, GWLP_WNDPROC, LONG_PTR(childWndProc));
         ::RemovePropA(hWnd, TEXT("__eureka_wndproc"));
         ::RemovePropA(hWnd, TEXT("__eureka_hwnd"));
      }
      break;

      case WM_KEYDOWN:
      {
         HWND clapHWND = (HWND) GetPropA(hWnd, TEXT("__eureka_hwnd"));
         Dwmprintf("[dbg] tkclap:WndProcedure_Sub: received WM_KEYDOWN lParam=0x%08x clapHWND=%p\n", lParam, clapHWND);
         ::SetWindowLongPtrA(clapHWND, 0/*OFF_bSubClassed*/, LONG_PTR(1));  // don't process again in WndProcedure()
         TKCLAPWindow *vw = tkclap_window_find_by_hwnd(clapHWND);
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
         Dwmprintf("[dbg] tkclap:WndProcedure_Sub: received WM_KEYUP lParam=0x%08x\n", lParam);
         HWND clapHWND = (HWND) GetPropA(hWnd, TEXT("__eureka_hwnd"));
         ::SetWindowLongPtrA(clapHWND, 0/*OFF_bSubClassed*/, LONG_PTR(1));
         TKCLAPWindow *vw = tkclap_window_find_by_hwnd(clapHWND);
         if(NULL != vw)
         {
            vw->plugin->callOnWin32KeyEvent((sUI)lParam);
         }
      }
      break;
   }

   // Call original window handler
   return ::CallWindowProc(childWndProc, hWnd, Msg, wParam, lParam);
}

// ---------------------------------------------------------------------------- WndProcedure
static LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,
                                     WPARAM wParam, LPARAM lParam
                                     ) {
#ifdef WM_DEBUG
   if(WM_TIMER != Msg)
   {
      fprintf(stderr, "tkclap:WndProcedure message:  ");
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
         Dwmprintf("[dbg] tkclap:WndProcedure: received WM_CREATE hWnd=%p\n", hWnd);
         return DefWindowProc(hWnd, Msg, wParam, lParam);

      case WM_PARENTNOTIFY:
         {
            Dwmprintf("[dbg] tkclap:WndProcedure: received WM_PARENTNOTIFY hWnd=%p\n", hWnd);
            // // ::SetFocus(hWnd);
            // ::EnableWindow(hWnd, TRUE);
            // ::SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE)&~WS_POPUP) | WS_CHILD);

            if(WM_CREATE == LOWORD(wParam))
            {
               // Hook WndProcedure (for keyboard events) and remember CLAP HWND and original wndproc
               HWND childHWND = HWND(lParam);
               WNDPROC childWndProc = (WNDPROC)GetWindowLongPtrA(childHWND, GWLP_WNDPROC);
               Dwmprintf("[dbg] tkclap:WndProcedure:  WM_CREATE childHWND=%p childWndProc=%p\n", childHWND, childWndProc);
               SetWindowLongPtrA(childHWND, GWLP_WNDPROC, LONG_PTR(&WndProcedure_Sub));
               SetWindowLongPtrA(hWnd, 0/*OFF_bSubClassed*/, LONG_PTR(0));
               SetProp(childHWND, TEXT("__eureka_hwnd"), HANDLE(hWnd));
               SetProp(childHWND, TEXT("__eureka_wndproc"), HANDLE(childWndProc));
            }
         }

         return DefWindowProc(hWnd, Msg, wParam, lParam);

      case WM_CLOSE:
         Dwmprintf("[dbg] tkclap::WndProcedure: received WM_CLOSE\n");
         {
            TKCLAPWindow *clapWin = tkclap_window_find_by_hwnd(hWnd);
            if((NULL != clapWin) && clapWin->b_allow_redirect_close_to_hide)
            {
               ::ShowWindow(hWnd, SW_HIDE);
            }
            else
            {
               // // Stop effIdle timer
               // KillTimer (hWnd, 1);

               // Destroy window and remove it from window list
               tkclap_window_remove_by_hwnd(hWnd);
            }
         }
         break;////return DefWindowProc(hWnd, Msg, wParam, lParam);

      // If the user wants to close the application
      case WM_DESTROY:
         Dwmprintf("[dbg] tkclap:WndProcedure: received WM_DESTROY\n");
         // then close it

         PostQuitMessage(WM_QUIT);
         break;

      case WM_TIMER:
      {
         // yac_host->printf("[dbg] tkclap:WndProcedure: received WM_TIMER\n");
         TKCLAPWindow *vw = tkclap_window_find_by_hwnd(hWnd);
         if(NULL != vw)
         {
            // // // vw->plugin->beginDispatch();
            // vw->plugin->effect->dispatcher(vw->plugin->effect, effEditIdle, 0, 0, 0, 0);
            // // // vw->plugin->endDispatch();
         }
      }
      break;

      // (note) [05Aug2019] usually handled in WndProcedure_Sub() but some plugins don't recv key events in Sub() handler
      //                     (todo) old VST comment, still valid ?
      case WM_KEYDOWN:
         Dwmprintf("[dbg] tkclap:WndProcedure: WM_KEYDOWN: bSubClassed=%p\n", bSubClassed);
         if(0 == bSubClassed)
         {
            Dwmprintf("[dbg] tkclap:WndProcedure: received WM_KEYDOWN lParam=0x%08x\n", lParam);
            TKCLAPWindow *vw = tkclap_window_find_by_hwnd(hWnd);
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
            Dwmprintf("[dbg] tkclap:WndProcedure: received WM_KEYUP lParam=0x%08x\n", lParam);
            TKCLAPWindow *vw = tkclap_window_find_by_hwnd(hWnd);
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

// ---------------------------------------------------------------------------- tkclap_RegisterClass
static ATOM tkclap_RegisterClass(HINSTANCE _dllHandle, const char *_className) {
   WNDCLASSEX WndClsEx;
   ATOM retAtom;

   // Create the application window
   WndClsEx.cbSize        = sizeof(WNDCLASSEX);
   WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
   WndClsEx.lpfnWndProc   = &WndProcedure;
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
   retAtom = ::RegisterClassEx(&WndClsEx);

   if(0 == retAtom)
   {
      yac_host->printf("[---] tkclap: RegisterClassEx() failed. LastError=%d\n", ::GetLastError());
   }

   return retAtom;
}

// ---------------------------------------------------------------------------- tkclap_window_build_window_title
void tkclap_window_build_window_title(CLAPPlugin *thiz) {
   // Generate window title (also used as class name)
   Dyac_snprintf(loc_init_window_title, TKCLAP_MAX_WINDOWTITLE_SIZE,
                 "%s / %s -- CLAP editor (thiz=%p)",
                 thiz->plugin_desc->vendor, thiz->plugin_desc->name, thiz
                 );
}

// ---------------------------------------------------------------------------- tkclap_window_get_geometry
void tkclap_window_get_geometry(TKCLAPWindow *vw, sSI *x, sSI *y, sSI *w, sSI *h) {
   RECT rect;
   ::GetWindowRect(vw->hWnd, &rect);
   yac_host->printf("[dbg] tkclap_window_get_geometry: rect=(%d; %d; %d; %d)\n",
                    rect.left,
                    rect.top,
                    rect.right,
                    rect.bottom
                    );
   *x = rect.left;
   *y = rect.top;
   *w = rect.right  - rect.left;
   *h = rect.bottom - rect.top;
}

// ---------------------------------------------------------------------------- tkclap_window_set_geometry
void tkclap_window_set_geometry(TKCLAPWindow *vw, sSI x, sSI y, sSI w, sSI h) {
   // ::SetWindowPos(vw->hWnd, HWND_TOP, x, y, w, h, SWP_NOMOVE);
#if 0
   RECT wRect;
   ::SetRect(&wRect, x, y, w, h);
   ::AdjustWindowRectEx(&wRect, ::GetWindowLong(vw->hWnd, GWL_STYLE), FALSE, ::GetWindowLong(hWnd, GWL_EXSTYLE));
#else
   RECT cr;
   RECT wr;
   POINT border;

   ::GetClientRect(vw->hWnd, &cr);
   ::GetWindowRect(vw->hWnd, &wr);

   border.x = (wr.right - wr.left) - cr.right;
   border.y = (wr.bottom - wr.top) - cr.bottom;

   ::SetWindowPos(vw->hWnd,
                  NULL/*hWndInsertAfter*/,
                  0/*x*/,
                  0/*y*/,
                  w + border.x,
                  h + border.y,
                  (
                     SWP_NOMOVE        |
                     SWP_NOOWNERZORDER |
                     SWP_NOZORDER      |
                     SWP_SHOWWINDOW    |
                     SWP_DRAWFRAME     |
                     0u
                   )
                  );
#endif
}

// ---------------------------------------------------------------------------- tkclap_window_set_visible
void tkclap_window_set_visible(TKCLAPWindow *vw, sBool bVisible) {
   if(bVisible)
      ::ShowWindow(vw->hWnd, SW_SHOWNORMAL);
   else
      ::ShowWindow(vw->hWnd, SW_HIDE);
}

// ---------------------------------------------------------------------------- tkclap_window_is_visible
sBool tkclap_window_is_visible(TKCLAPWindow *vw) {
   LONG info = GetWindowLong(vw->hWnd, GWL_STYLE);
   return (0 != (info & WS_VISIBLE));
}

// ---------------------------------------------------------------------------- tkclap_window_close
void tkclap_window_close(TKCLAPWindow *vw) {
   ::PostMessage(vw->hWnd, WM_CLOSE, 0, 0);

   yac_host->printf("[dbg] tkclap_window_close: wait until editor window has been closed.\n");
   int iter = 0;
   while(tkclap_window_find_by_plugin(vw->plugin, YAC_TRUE/*bLock*/))
   {
      yac_host->printf("...\n");

      // Flush event queue
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
   yac_host->printf("[dbg] tkclap_window_close: OK, editor window has been closed.\n");
}

// ---------------------------------------------------------------------------- tkclap_window_create
TKCLAPWindow *tkclap_window_create(CLAPPlugin *thiz) {

   Dprintf("[dbg] tkclap_window_create: ENTER.\n");

   TKCLAPWindow *vw = NULL;

   // (note) inits loc_init_window_title (also used as window class name)
   tkclap_window_build_window_title(thiz);

   sBool bParent = YAC_TRUE;  // create top-level window and call ext_gui->set_parent() to embed plugin UI
   // sBool bParent = YAC_FALSE;  // let plugin create floating top-level window (does not seem to work with any plugin)

   if(1)
   {
      // Create plugin editor
      if(thiz->ext_gui->create(thiz->plugin, bParent?CLAP_WINDOW_API_WIN32:"", bParent?0:1/*is_floating=0=>embed in parent*/))
      {
         Dprintf("[dbg] tkclap_window_create: ext_gui->create() OK\n");

         if(bParent)
         {
#if 0
            Dprintf("[dbg] tkclap_window_create: call ext_gui->set_scale(1)\n");
            if(thiz->ext_gui->set_scale(thiz->plugin, 1.0))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->set_scale() OK\n");
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->set_scale() failed\n");
            }
#endif

            Dprintf("[dbg] tkclap_window_create: call ext_gui->get_size()\n");
            sU32 sizeX = 0u;
            sU32 sizeY = 0u;
            if(thiz->ext_gui->get_size(thiz->plugin, &sizeX, &sizeY))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->get_size() returned (%u,%u)\n", sizeX, sizeY);
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->get_size() failed\n");
            }

            if(sizeX < 100u)
               sizeX = 100u;
            if(sizeY < 100u)
               sizeY = 100u;

            Dprintf("[dbg] tkclap_window_create: creating window with size=(%u,%u)\n", sizeX, sizeY);

            // Create toplevel window
            HINSTANCE hInstance = ::GetModuleHandle(NULL);
            ATOM atom = tkclap_RegisterClass(hInstance, loc_init_window_title/*className*/);
            if(0 != atom)
            {
               // Create the window object
               HWND hWnd = ::CreateWindow(loc_init_window_title/*lpClassName*/,
                                          loc_init_window_title/*lpWindowName*/,
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
                                          NULL/*hWndParent*/,  ////(HWND)thiz->parent_hwnd
                                          NULL/*hMenu*/,
                                          hInstance,
                                          NULL/*lpParam*/
                                          );

               yac_host->printf("[dbg] tkclap_window_create: hWnd=0x%p\n", hWnd);

               if(NULL != hWnd )
               {
                  sBool bOK = 1;

                  SetWindowLongPtrA(hWnd, 0/*bSubClassed*/, LONG_PTR(0));
                  ::SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)&~WS_SIZEBOX);

                  if(bOK)
                  {
                     // Remember in window list
                     vw = new(std::nothrow) TKCLAPWindow;
                     tkclap_window_lock();
                     vw->next                           = first_window;
                     vw->hWnd                           = hWnd;
                     vw->plugin                         = thiz;
                     vw->b_allow_redirect_close_to_hide = YAC_TRUE;
                     ::strncpy(vw->window_title, loc_init_window_title, TKCLAP_MAX_WINDOWTITLE_SIZE);
                     first_window = vw;
                     tkclap_window_unlock();

                     // Set plugin editor parent
                     clap_window_t clapParentWindow;
                     clapParentWindow.api   = CLAP_WINDOW_API_WIN32;
                     clapParentWindow.win32 = vw->hWnd;
                     if(thiz->ext_gui->set_parent(thiz->plugin, &clapParentWindow))
                     {
                        Dprintf("[dbg] tkclap_window_create: ext_gui->set_parent() OK\n");
                     }
                     else
                     {
                        Dprintf("[~~~] tkclap_window_create: ext_gui->set_parent() failed\n");
                     }

                     // Flush event queue
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

                     {
                        RECT wRect;
                        ::SetRect(&wRect, 0, 0, sizeX, sizeY);
                        ::AdjustWindowRectEx(&wRect, ::GetWindowLong(hWnd, GWL_STYLE), FALSE, ::GetWindowLong(hWnd, GWL_EXSTYLE));
                        sizeX = wRect.right  - wRect.left;
                        sizeY = wRect.bottom - wRect.top;
                     }

                     if(thiz->ui_window_w >= 100)
                     {
                        // Restore window geometry
                        Dprintf("[dbg] tkclap_window_create: restore geo=(%d; %d; %d; %d)\n",
                                thiz->ui_window_x, thiz->ui_window_y,
                                thiz->ui_window_w, thiz->ui_window_h
                                );
                        tkclap_window_set_geometry(vw,
                                                   thiz->ui_window_x, thiz->ui_window_y,
                                                   thiz->ui_window_w, thiz->ui_window_h
                                                   );
                     }
                     else
                     {
                        tkclap_window_get_geometry(vw,
                                                   &thiz->ui_window_x, &thiz->ui_window_y,
                                                   &thiz->ui_window_w, &thiz->ui_window_h
                                                   );
                        Dprintf("[dbg] tkclap_window_create: query geo=(%d; %d; %d; %d)\n",
                                thiz->ui_window_x, thiz->ui_window_y,
                                thiz->ui_window_w, thiz->ui_window_h
                                );
                     }

                     // Use default position and size
                     ::SetWindowPos(hWnd, HWND_TOP, 0, 0, sizeX, sizeY, SWP_NOMOVE);

                     // Display the window to the user and focus it
                     ::ShowWindow(hWnd, SW_SHOWNORMAL);
                     ::UpdateWindow(hWnd);
                     ::SetFocus(hWnd);
                     ::SetActiveWindow(hWnd);
                  }
               } // if hWnd
               else
               {
                  yac_host->printf("[---] tkclap_window_create: CreateWindow() failed (LastError=%d).\n", GetLastError());
                  ::UnregisterClass(loc_init_window_title, hInstance);
               }
            } // if atom
            else
            {
               yac_host->printf("[---] tkclap_window_create: RegisterClassEx() failed (LastError=%d).\n", GetLastError());
            }

         } // if bParent
         else
         {
            // (note) none of the tested plugins seem to work with floating windows
            //         (either ext_gui->create() fails or the window is not visible, despite ext_gui->show())

            // set_transient
            if(NULL != thiz->transient_native_window_handle)
            {
               clap_window_t windowBelow;
               ::memset((void*)&windowBelow, 0, sizeof(clap_window_t));
               windowBelow.api = CLAP_WINDOW_API_WIN32;
               windowBelow.win32 = (HWND)thiz->transient_native_window_handle;
               if(thiz->ext_gui->set_transient(thiz->plugin, &windowBelow))
               {
                  Dprintf("[~~~] tkclap_window_create: ext_gui->set_transient() OK\n");
               }
               else
               {
                  Dprintf("[~~~] tkclap_window_create: ext_gui->set_transient() failed\n");
               }
            }

            // suggest_title
            char windowTitle[512];
            snprintf(windowTitle, 512, "CLAP editor [%s]", thiz->plugin_desc->name);
            Dprintf("[dbg] tkclap_window_create: call ext_gui->suggest_title()\n");
            thiz->ext_gui->suggest_title(thiz->plugin, windowTitle);
         }
      }
      else
      {
         Dprintf("[~~~] tkclap_window_create: ext_gui->create() failed\n");
      }
   }

   Dprintf("[dbg] tkclap_window_create: LEAVE. vw=%p\n", vw);
   return vw;
}

// ---------------------------------------------------------------------------- tkclap_window_find_by_plugin
TKCLAPWindow *tkclap_window_find_by_plugin(CLAPPlugin *_plugin, sBool _bLock) {
   if(_bLock)
      tkclap_window_lock();

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->plugin == _plugin)
         break;
      else
         vw = vw->next;
   }

   if(_bLock)
      tkclap_window_unlock();

   return vw;
}

// ---------------------------------------------------------------------------- tkclap_window_remove_by_hwnd
void tkclap_window_remove_by_hwnd(HWND _hWnd) {
   // called by WM_CLOSE (WndProcedure)

   Dprintf("[dbg] tkclap_window_remove_by_hwnd: hWnd=%p\n", _hWnd);

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->hWnd == _hWnd)
      {
         // Remember window position and size
         {
            Dprintf("[dbg] tkclap_window_remove_by_hwnd: caching window geometry\n");
            tkclap_window_get_geometry(vw,
                                       &vw->plugin->ui_window_x, &vw->plugin->ui_window_y,
                                       &vw->plugin->ui_window_w, &vw->plugin->ui_window_h
                                       );
            Dprintf("[dbg] tkclap_window_remove_by_hwnd: geo=(%d; %d; %d; %d)\n",
                    vw->plugin->ui_window_x, vw->plugin->ui_window_y,
                    vw->plugin->ui_window_w, vw->plugin->ui_window_h
                    );
         }

         // Free plugin editor resources
         Dprintf("[dbg] tkclap_window_remove_by_hwnd: call ext_gui->destroy()\n");
         vw->plugin->ext_gui->destroy(vw->plugin->plugin);
         Dprintf("[dbg] tkclap_window_remove_by_hwnd: ext_gui->destroy() returned\n");

         Dprintf("[dbg] tkclap_window_remove_by_hwnd: window hWnd=0x%p destroyed\n", _hWnd);

         // Close/destroy window
         ::DestroyWindow(vw->hWnd);
         vw->hWnd = NULL;

         // Unregister class
         HINSTANCE hInstance = ::GetModuleHandle(NULL);
         yac_host->printf("[dbg] tkclap_window_remove_by_hwnd: UnregisterClass(\"%s\")\n", vw->window_title);
         if(!::UnregisterClass(vw->window_title, hInstance))
         {
            yac_host->printf("[---] tkclap_window_remove_by_hwnd: UnregisterClass() failed. LastError=%d\n", ::GetLastError());
         }

         // Remove from window list
         tkclap_window_lock();
         if(vw == first_window)
         {
            // Unlink from window list (first window)
            first_window = vw->next;
         }
         else
         {
            // Unlink from window list
            TKCLAPWindow *p = first_window;
            while(p->next != vw)
               p = p->next;
            p->next = vw->next;
         }
         // Free memory for TKCLAPWindow
         delete vw;
         tkclap_window_unlock();

         break;
      }
      else
      {
         vw = vw->next;
      }
   } // while vw

}
