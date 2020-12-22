/* See LICENSE file for copyright and license details. */

/* Constants */
#define TERMINAL "st"
#define TERMCLASS "St"

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static const int gappx              = 20;        /* gaps between windows */
static unsigned int snap      = 32;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char *fonts[]          = { "IBMPlexMono:size=10:antialias=True:autohint=True", "Font Awesome 5 Free:pixelsize=10:antialias=true:autohint=true" };
static char dmenufont[]       = "monospace:size=10";
static char normbgcolor[]     = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[]     = "#bbbbbb";
static char selfgcolor[]      = "#eeeeee";
static char selbordercolor[]  = "#770000";
static char selbgcolor[]      = "#005577";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static char *colors[][3]      = {
	/*               fg             bg              border   */
	[SchemeNorm] = { normfgcolor,   normbgcolor,    normbordercolor },
	[SchemeSel]  = { selfgcolor,    selbgcolor,     selbordercolor  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char *tagsalt[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor    scratchkey*/
	{ "Gimp",     NULL,       NULL,       0,            1,           -1,        0 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1,        0 },
	{ NULL,       NULL,      "stsp",      0,            1,           -1,       't'},
	{ NULL,       NULL,      "lfsp",      0,            1,           -1,       'l'},
	{"ramboxpro", NULL,       NULL,       0,            1,           -1,       'r'},
	{"Spotify",   NULL,       NULL,       0,            1,           -1,       's'},
	{"Mailspring",NULL,       NULL,       0,            1,           -1,       'm'},
	{ NULL,       NULL,      "qnsp",      0,            1,           -1,       'q'},
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */

#include "bottomstack.c"
#include "centeredmaster.c"
#include "fibonacci.c"
#include "gaplessgrid.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "###",      gaplessgrid },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
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
static const char *roficmd[] = { "rofi", "-show", "combi", NULL};
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "color0",             STRING,  &normbgcolor },
		{ "color0",             STRING,  &normbordercolor },
		{ "color4",             STRING,  &normfgcolor },
		{ "color4",             STRING,  &selbgcolor },
		{ "color8",             STRING,  &selbordercolor },
		{ "color0",             STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

/*First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = {"t", "st", "-t", "stsp", NULL};
static const char *rbscratchpadcmd[] = {"r", "ramboxspawn", NULL};
static const char *lfscratchpadcmd[] = {"l", "st", "-t", "lfsp", "-e", "lf", NULL};
static const char *spscratchpadcmd[] = {"s", "spotifyspawn", NULL};
static const char *msscratchpadcmd[] = {"m", "mailspring", NULL};
static const char *qnscratchpadcmd[] = {"q", "st", "-t", "qnsp", "-e", "quicknote", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = roficmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, togglescratch,  {.v = scratchpadcmd } },
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
	{ MODKEY,                       XK_r,      reorganizetags, {0} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, /* tiled */
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} }, /* floating */
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[2]} }, /* monocle */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} }, /* bottomstack */
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[4]} }, /* focusedmaster */
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[5]} }, /* fmasterfloat */
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[6]} }, /* gaplessgrid */
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[7]} }, /* spiral */
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[8]} }, /* dwindle */
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask,             XK_b,      togglealttag,   {0} },
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
