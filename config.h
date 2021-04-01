/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx             = 0;        /* border pixel of windows */
static unsigned int snap                 = 32;       /* snap pixel */
static const unsigned int gappih         = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv         = 10;       /* vert inner gap between windows */
static const unsigned int gappoh         = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov         = 30;       /* vert outer gap between windows and screen edge */
static int smartgaps                     = 0;        /* 1 means no outer gap when there is only one window */
static int showbar                       = 1;        /* 0 means no bar */
static int topbar                        = 1;        /* 0 means bottom bar */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray             = 1;   /* 0 means no systray */
static int floatposgrid_x           = 5;        /* float grid columns */
static int floatposgrid_y           = 5;        /* float grid rows */
static const char *fonts[]          = { "IBMPlexMono:size=10:antialias=True:autohint=True", "Font Awesome 5 Free:pixelsize=10:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static const unsigned int baralpha = 255;
static const unsigned int borderalpha = OPAQUE;
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   floatpos   monitor    scratch key */
	{ "Gimp",     NULL,       NULL,       0,            1,           NULL,      -1,        0 },
	{ "firefox",  NULL,       NULL,       0,            0,           NULL,      -1,       'w'},
	{ NULL,       NULL,      "stsp",      0,            1,          "50% 50%",  -1,       't'},
	{ NULL,       NULL,      "lfsp",      0,            1,          "50% 50%",  -1,       'l'},
	{"ramboxpro", NULL,       NULL,       0,            1,          "50% 50%",  -1,       'r'},
	{ NULL,       NULL,      "sptsp",     0,            1,          "50% 50%",  -1,       's'},
	{"Mailspring",NULL,       NULL,       0,            1,          "50% 50%",  -1,       'm'},
	{ NULL,       NULL,      "qnsp",      0,            1,          "50% 50%",  -1,       'q'},
	/* {"Vivaldi-stable",NULL,   NULL,       0,            0,           NULL,      -1,       'w'}, */

};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static int attachbelow = 1;    /* 1 means attach after the currently active window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "D[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
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

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *roficmd[] = { "rofi", "-show", "combi", NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *termattachcmd[]  = { "alacritty", "-e", "tmuxdd", NULL };
static const char *termnewcmd[]  = { "alacritty", "-e", "tmuxnd", NULL };

/*First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = {"t", "alacritty", "-t", "stsp", "-e", "tmuxsp", NULL};
static const char *rbscratchpadcmd[] = {"r", "ramboxpro", NULL};
static const char *lfscratchpadcmd[] = {"l", "alacritty", "-t", "lfsp", "-e", "lf", NULL};
static const char *spscratchpadcmd[] = {"s", "alacritty", "-t", "sptsp", "-e", "sptspawn", NULL};
static const char *msscratchpadcmd[] = {"m", "mailspring", NULL};
static const char *qnscratchpadcmd[] = {"q", "alacritty", "-t", "qnsp", "-e", "quicknote", NULL};
static const char *ffscratchpadcmd[] = {"w", "firefox", NULL};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "color0",             STRING,  &normbgcolor },
		{ "color0",             STRING,  &normbordercolor },
		{ "color3",             STRING,  &normfgcolor },
		{ "color0",             STRING,  &selbgcolor },
		{ "color8",             STRING,  &selbordercolor },
		{ "color3",             STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = roficmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termattachcmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termnewcmd } },
	{ MODKEY|ControlMask,           XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY|ShiftMask,             XK_grave,  removescratch,  {.v = scratchpadcmd } },
	{ MODKEY|ControlMask,           XK_grave,  setscratch,     {.v = scratchpadcmd } },
	{ MODKEY,                       XK_w,      togglescratch,  {.v = ffscratchpadcmd } },
	{ MODKEY|ShiftMask,             XK_w,      removescratch,  {.v = ffscratchpadcmd } },
	{ MODKEY|ControlMask,           XK_w,      setscratch,     {.v = ffscratchpadcmd } },
	{ MODKEY,                       XK_a,      togglescratch,  {.v = rbscratchpadcmd } },
	{ MODKEY,                       XK_e,      togglescratch,  {.v = lfscratchpadcmd } },
	{ MODKEY,                       XK_s,      togglescratch,  {.v = spscratchpadcmd } },
	{ MODKEY,                       XK_m,      togglescratch,  {.v = msscratchpadcmd } },
	{ MODKEY,                       XK_z,      togglescratch,  {.v = qnscratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                    XK_BackSpace, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, // tiled
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[5]} }, // bottomstack
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[2]} }, // spiral
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[3]} }, // dwindle
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[11]} }, // centeredmaster
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[12]} }, // centeredmasterfloat
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[10]} }, // gaplessgrid
	{ MODKEY|ShiftMask,             XK_g,      setlayout,      {.v = &layouts[7]} }, // grid
	{ MODKEY|ShiftMask,             XK_d,      setlayout,      {.v = &layouts[4]} }, // grid
	{ MODKEY,                       XK_Insert, incrgaps,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Insert, incrgaps,       {.i = +1 } },
	{ MODKEY,                       XK_Delete, incrigaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Delete, incrigaps,      {.i = +1 } },
	{ MODKEY,                       XK_End,    incrogaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_End,    incrogaps,      {.i = +1 } },
//	{ MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },
//	{ MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
//	{ MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },
//	{ MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
//	{ MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },
//	{ MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
//	{ MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },
//	{ MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY,                       XK_Home,   togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_Home,   defaultgaps,    {0} },
	{ MODKEY|ShiftMask,          XK_BackSpace, togglefloating, {0} },
	{ MODKEY,                       XK_f,      togglefullscreen, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefakefullscreen, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_comma,  tagallmon,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_period, tagallmon,      {.i = -1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ControlMask|ShiftMask, XK_F4,     quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

