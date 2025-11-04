/* See LICENSE file for copyright and license details. */

/* program constants */
#define TERMINAL "st"
#define TERMINAL_CLASS "st-256color"

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static unsigned int gappih    = 10;	/* horiz inner gap between windows */
static unsigned int gappiv    = 10;	/* vert inner gap between windows */
static unsigned int gappoh    = 10;	/* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 10;	/* vert outer gap between windows and screen edge */
static int smartgaps	    = 0;	/* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char *fonts[]          = { "monospace:size=10", "NotoColorEmoji:pixelsize=10:antialias=true:autohint=true" };
static char dmenufont[]       = "monospace:size=10";

/* custom default colors */
static char normbgcolor[] 	= "#2e3440"; /* #2e3440 */
static char normbordercolor[] 	= "#34402e"; /* #4c566a */
static char normfgcolor[] 	= "#e29e32"; /* #d8dee9 */
static char selfgcolor[]	= "#eceff4"; /* #eceff4 */
static char selbordercolor[]	= "#a3be8c"; /* #a3be8c */
static char selbgcolor[]	= "#3a2e40"; /* #b48ead */

static char *colors[][3]      = {
	/*                   fg         bg         border   */
	[SchemeNorm] 	 = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]	 = { selbgcolor, selfgcolor,  selbordercolor  },
	/* colorbar patch: {   text,      bg,        unused  } */
	[SchemeStatus]	 = { normfgcolor, 	normbgcolor, "#000000" },
	[SchemeTagsSel]  = { normfgcolor, 	normbgcolor,  "#000000" },
	[SchemeTagsNorm] = { selbordercolor, normbgcolor, "#000000" },
	[SchemeInfoSel]  = { normfgcolor, 	normbgcolor,  "#000000" },
	[SchemeInfoNorm] = { normfgcolor, 	normbgcolor, "#000000" },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = { TERMINAL, "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = { TERMINAL, "-n", "spcalc", "-f", "monospace:size=14", "-g", "50x20", "-e", "bc", "-lq", NULL };
static Sp scratchpads[] = {
	/* name		cmd */
	{"spterm",	spcmd1},
	{"spcalc",	spcmd2},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "steam",    NULL,       NULL,       1 << 8,            0,           -1 },
	{ TERMINAL_CLASS, "spterm", NULL,   SPTAG(0),     1,	         -1 },
	{ TERMINAL_CLASS, "spcalc", NULL,   SPTAG(1),     1,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { TERMINAL, NULL };
static const char *mutecmd[] = { "/bin/sh", "-c", "wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle; kill -44 $(pidof dwmblocks)" };
static const char *incvolcmd[] = { "/bin/sh", "-c", "wpctl set-volume @DEFAULT_AUDIO_SINK@ 0%- && wpctl set-volume @DEFAULT_AUDIO_SINK@ 3%+; kill -44 $(pidof dwmblocks)" };
static const char *decvolcmd[] = { "/bin/sh", "-c", "wpctl set-volume @DEFAULT_AUDIO_SINK@ 0%+ && wpctl set-volume @DEFAULT_AUDIO_SINK@ 3%-; kill -44 $(pidof dwmblocks)" };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "normbgcolor",	STRING,		&normbgcolor },
	{ "normbordercolor",	STRING,		&normbordercolor },
	{ "normfgcolor",	STRING,		&normfgcolor },
	{ "selbgcolor",		STRING,		&selbgcolor },
	{ "selbordercolor",	STRING,		&selbordercolor },
	{ "selfgcolor",		STRING,		&selfgcolor },
	{ "borderpx",		INTEGER,	&borderpx },
	{ "snap",		INTEGER,	&snap },
	{ "showbar",		INTEGER,	&showbar },
	{ "topbar",		INTEGER,	&topbar },
	{ "nmaster",		INTEGER,	&nmaster },
	{ "resizehints",	INTEGER, 	&resizehints },
	{ "mfact",		FLOAT,		&mfact },
};

#include <X11/XF86keysym.h>

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,	                XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,			XK_w,	   spawn,	   {.v = (const char *[]) { "firefox", NULL } } },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_i,      incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,	                XK_q,      killclient,     {0} }, 	// close current window
	{ MODKEY|ControlMask,		XK_q,	   killclient,	   {.ui = 1} }, // close all except current
	{ MODKEY|ShiftMask|ControlMask, XK_q,	   killclient,	   {.ui = 2} }, // close all
	{ MODKEY,			XK_l,	   spawn	   {.v = (const char *[]) { "lock", NULL } } }, // lock screen 

	// SCRATCHPAD
	{ MODKEY|ShiftMask,		XK_Return, togglescratch,  {.ui = 0} }, // sp terminal
	{ MODKEY,			XK_apostrophe, togglescratch, {.ui = 1} }, // sp calculator

	// GAP CONTROLS
	{ MODKEY|Mod1Mask,		XK_minus,  incrgaps,	   {.i = -2} },  /* decrease all gaps */
	{ MODKEY|Mod1Mask,		XK_equal,  incrgaps,	   {.i = +2} },  /* increase all gaps */
	{ MODKEY,			XK_g,	   togglegaps,	   {0} },	 /* toggle gaps */
	{ MODKEY|ShiftMask,		XK_g,	   defaultgaps,	   {0} },	 /* return gaps to default settings */
	{ MODKEY|Mod1Mask,		XK_i,	   incrigaps,	   {.i = +1 } }, /* inner inc */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,	   incrigaps,	   {.i = -1 } }, /* inner dec */
	{ MODKEY|Mod1Mask,		XK_o,	   incrogaps,	   {.i = +1 } }, /* outer inc */
	{ MODKEY|Mod1Mask|ShiftMask,	XK_o,	   incrogaps,	   {.i = -1 } }, /* outer dec */
	{ MODKEY|Mod1Mask,		XK_1,	   incrihgaps,	   {.i = +1 } }, /* inner horiz inc */
	{ MODKEY|Mod1Mask|ShiftMask,	XK_1,	   incrihgaps,	   {.i = -1 } }, /* inner horiz dec */
	{ MODKEY|Mod1Mask,		XK_2,	   incrivgaps,	   {.i = +1 } }, /* inner vert inc */
	{ MODKEY|Mod1Mask|ShiftMask,	XK_2,	   incrivgaps,	   {.i = -1 } }, /* inner vert dec */
	{ MODKEY|Mod1Mask,		XK_3,	   incrohgaps,	   {.i = +1 } }, /* outer horiz inc */
	{ MODKEY|Mod1Mask|ShiftMask,	XK_3,	   incrohgaps,	   {.i = -1 } }, /* outer horiz dec */
	{ MODKEY|Mod1Mask,		XK_4,	   incrovgaps,	   {.i = +1 } }, /* outer vert inc */
	{ MODKEY|Mod1Mask|ShiftMask,	XK_4,	   incrovgaps,	   {.i = -1 } }, /* outer vert dec */

	// CUSTOM SCRIPTS
	{ MODKEY,			XK_BackSpace, spawn,	   {.v = (const char *[]) { "sysop", NULL} } }, // system control options
	{ MODKEY,			XK_grave,  spawn,	   {.v = (const char *[]) { "symbolist", NULL } } }, // unicode icon picker

	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,           XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_Right,  viewnext,       {0} },
	{ MODKEY,                       XK_Left,   viewprev,       {0} },
	{ MODKEY|ShiftMask,             XK_Right,  tagtonext,      {0} },
	{ MODKEY|ShiftMask,             XK_Left,   tagtoprev,      {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	// Function keys (F1-F12)
	{ MODKEY,			XK_F5,	xrdb,		   {.v = NULL } },

	// Special keyboard keys
	{ 0,     			XF86XK_MonBrightnessUp,	spawn, {.v = (const char *[]) { "xbacklight", "-inc", "10", NULL } } },
	{ 0,				XF86XK_MonBrightnessDown,  spawn, {.v = (const char *[]) { "xbacklight", "-dec", "10", NULL } } },
	{ 0,				XF86XK_AudioMute, spawn,   {.v = mutecmd } },
	{ 0,				XF86XK_AudioRaiseVolume, spawn, {.v = incvolcmd } },
	{ 0,				XF86XK_AudioLowerVolume, spawn, {.v = decvolcmd } },
	{ 0,				XF86XK_Calculator, togglescratch, {.ui = 1 } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,	0,		Button1,	sigstatusbar,	{.i = 1} },
	{ ClkStatusText,	0,		Button2,	sigstatusbar,	{.i = 2} },
	{ ClkStatusText,	0,		Button3,	sigstatusbar,	{.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

