#include <X11/XF86keysym.h>
#include "movestack.c"

/* See LICENSE file for copyright and license details. */
/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const char *fonts[]          = { "Source Code Pro:size=15" };
static const char dmenufont[]       = "Source Code Pro:size=15";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#37474F";
static const char col_border[]      = "#FF75BC";
static const unsigned int baralpha = 0xd0;
//static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border  },
	//[SchemeHid]  = { col_cyan,  col_gray1, col_border  },
};

// enable system volume
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };

/* tagging */
//static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };
static const char *tags[] = { "","","","","","", ""};
static const char *tagsalt[] = { "一", "二", "三", "四", "五", "六", "七"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                 instance    title       tags mask     isfloating   monitor */
	{ "Android Emulator",    NULL,       NULL,       0,            1,           -1 },
	{ "Emulator",            NULL,       NULL,       0,            1,           -1 },
	{ "quemu-system-i386",   NULL,       NULL,       0,            1,           -1 },
	{ "netease-cloud-music", NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",             NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

static const char scratchpadname[] = "scratchpad";

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };

/* apps */
static const char *termcmd[]  = { "alacritty", NULL };
static const char *chrome[] = { "chromium", NULL };
static const char *rofi[] = { "rofi", "-show", "run", NULL };
static const char *rofi_copy[] = { "rofi", "-modi", "clipboard:greenclip print", "-show", "clipboard", "-run-command", "'{cmd}'", NULL };
static const char *flameshot[] = { "flameshot", "gui", NULL };

/* customization */
static const char *suspendcmd[]  = { "/home/lingao/scripts/suspend.sh", NULL };
static const char *screenlockcmd[]  = { "/home/lingao/scripts/screenlock.sh", NULL};
static const char *incbacklightcmd[]  = { "/home/lingao/scripts/inc-backlight.sh", NULL};
static const char *decbacklightcmd[]  = { "/home/lingao/scripts/dec-backlight.sh", NULL};
static const char *sktogglecmd[]  = { "/home/lingao/scripts/sck-tog.sh", NULL };
static const char *nautilustogcmd[]  = { "/home/lingao/scripts/nautilus-tog.sh", NULL };

static Key keys[] = {
	/* customized key-bindings */
	{ MODKEY,                       XK_g,      spawn,          {.v = chrome } },
	{ MODKEY,                       XK_p,      spawn,          {.v = rofi } },
	{ MODKEY,                       XK_y,      spawn,          {.v = rofi_copy } },
	{ MODKEY|ControlMask,           XK_a,      spawn,          {.v = flameshot } },
	//{ MODKEY|ControlMask,           XK_Return,  togglescratch,  {.v = scratchpadcmd } },
    { MODKEY,             XK_plus,      aspectresize,   {.i = +24} },
	{ MODKEY,             XK_minus,      aspectresize,   {.i = -24} },
	/* modified keys */
	{ MODKEY,	                     XK_w,      killclient,     {0} },
	{ MODKEY,                        XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,              XK_comma,                spawn,          {.v = decbacklightcmd } },
	{ MODKEY,              XK_period,               spawn,          {.v = incbacklightcmd } },
	{ MODKEY|ControlMask,  XK_s,                    spawn,          {.v = sktogglecmd } },
	{ MODKEY,              XK_f,                    spawn,          {.v = nautilustogcmd } },

    // system control
	{ 0,                       XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ 0,                       XF86XK_AudioMute, spawn, {.v = mutevol } },
	{ 0,                       XF86XK_AudioRaiseVolume, spawn, {.v = upvol   } },
	/* modifier                     key        function        argument */
	//{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_h,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_l,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_l,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_j,      movestack,      {.i = +1 } },
	{ MODKEY,                       XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Left,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_k,    setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_j,      setcfact,       {.f = -0.25} },
    { MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_n,      togglealttag,   {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
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
