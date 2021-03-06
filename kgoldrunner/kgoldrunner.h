/***************************************************************************
                          kgoldrunner.h  -  description
                             -------------------
    begin                : Wed Jan 23 15:19:17 EST 2002
    copyright            : (C) 2002 by Marco Kr�ger and Ian Wadham
    email                : See menu "Help, About KGoldrunner"
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGOLDRUNNER_H
#define KGOLDRUNNER_H

// Enable the next "#define" or use compiler option -DQT1 if you want to compile
// with Qt Library v1.x and KDE v1.x desktop.  Otherwise, we will use Qt v2.x
// and no particular desktop (thus moving towards independence of O/S).
//
// #define QT1

// Macros to smooth out the differences between Qt 1 and Qt 2 classes.
//
// "myStr" converts a QString object to a C language "char*" character string.
// "myChar" extracts a C language character (type "char") from a QString object.
// "endData" checks for an end-of-file condition.
//
#define myStr		data
#define myChar(i)	at((i))
#define endData		eof

/******************************************************************************/
/*****************************    INCLUDEs     ********************************/
/******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <ctype.h>

#include <kapp.h>
#include <kmsgbox.h>

#include <qmessagebox.h>
#include <qdatetime.h>

#include "kgrobj.h"

#include <ktopwidget.h>

#include <qcolor.h>
#include <qkeycode.h>
#include <qlist.h>
#include <qstring.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>

#include <qdialog.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmultilinedit.h>

class KGoldrunnerWidget;	// Forward declare the central widget class.

/******************************************************************************/
/******************************   CONSTANTS   *********************************/
/******************************************************************************/

// DateiMenue (Game Menu)
const int ID_GAMEMENU =  100;
const int ID_NEW =  101;
const int ID_OPEN = 102;
const int ID_SAVE = 103;
const int ID_EXIT = 104;
const int ID_NEXT = 105;
const int ID_KILL = 106;
const int ID_SAVEGAME = 107;
const int ID_LOADGAME = 108;
const int ID_HIGH = 110;

// Pause/Resume Toggle
const int ID_PAUSE = 109;

// Edit Menu
const int ID_EDITMENU = 300;
const int ID_CREATE   = 301;
const int ID_UPDATE   = 302;
const int ID_EDITNEXT = 303;
const int ID_SAVEFILE = 304;
const int ID_MOVEFILE = 305;
const int ID_DELEFILE = 306;
const int ID_ECOLL    = 307;
const int ID_NCOLL    = 308;

// Settings Menu
const int ID_OPT      = 400;
const int ID_KGR      = 401;
const int ID_TRAD     = 402;
const int ID_MESLOW   = 403;
const int ID_ACGOLD   = 404;
const int ID_RTHOLE   = 405;
const int ID_RATOP    = 406;
const int ID_SRCH1    = 407;
const int ID_SRCH2    = 408;
const int ID_NSPEED   = 409;
const int ID_BSPEED   = 410;
const int ID_VSPEED   = 411;
const int ID_CSPEED   = 412;
const int ID_ISPEED   = 413;
const int ID_DSPEED   = 414;

// HilfeMenue (Help Menu)
const int ID_HELPMENU = 200;
const int ID_TUTE     = 201;
const int ID_HINT     = 202;
const int ID_MANUAL   = 203;
const int ID_ABOUTKGR = 204;
const int ID_ABOUTQT  = 205;

// Statusbar
const int ID_LIFES      = 901;		// ID's of entries in KDE 1 status bar.
const int ID_SCORE      = 902;
const int ID_LEVEL      = 903;
const int ID_LEVELPLACE = 904;
const int ID_DUMMY      = 905;

// Edit toolbar
const int ID_FREE	= 810;
const int ID_HERO	= 811;
const int ID_ENEMY	= 812;
const int ID_LADDER	= 813;
const int ID_HLADDER	= 814;
const int ID_BRICK	= 815;
const int ID_BETON	= 816;
const int ID_FBRICK	= 817;
const int ID_POLE	= 818;
const int ID_NUGGET	= 819;

enum Owner {SYSTEM, USER};

/******************************************************************************/
/***********************    COLLECTION DATA CLASS    **************************/
/******************************************************************************/

class KGrCollection
{
public:
    KGrCollection (Owner o, const QString & n, const QString & p,
		   const char s, int nl, const QString & a);
    Owner	owner;		// Collection owner: "System" or "User".
    QString	name;		// Collection name.
    QString	prefix;		// Collection's filename prefix.
    char	settings;	// Collection rules: KGoldrunner or Traditional.
    int		nLevels;	// Number of levels in the collection.
    QString	about;		// Optional text about the collection.
};

/******************************************************************************/
/**********************    CLASS TO DISPLAY THUMBNAIL   ***********************/
/******************************************************************************/

class KGrThumbNail : public QFrame
{
public:
    KGrThumbNail (QWidget *parent = 0, const char *name = 0);
    void setFilePath (QString &, QLabel *);	// Set filepath and name field.
protected:
    void drawContents (QPainter *);		// Draw a preview of a level.
    QString filePath;
    QLabel * lName;
};

/******************************************************************************/
/**********************    KGOLDRUNNER (MAIN) CLASS    ************************/
/******************************************************************************/

class KGoldrunner : public KTopLevelWidget		// QT1_ONLY
{

	Q_OBJECT
public:
	KGoldrunner(QWidget *parent = 0, const char *name = 0, WFlags = 0);
	virtual ~KGoldrunner();

/******************************************************************************/
/***********************  MENUS AND APPLICATION CONTROL  **********************/
/******************************************************************************/

private:
	// "getDirectories()" is HIGHLY DEPENDENT on the operating system and
	// desktop environment.  See the implementation code for clues as to
	// where the KGoldrunner files are physically located.

	bool getDirectories ();		// Get directory paths, as below.
	QString systemHTMLDir;		// Where the manual is stored.
	QString systemDataDir;		// Where the system levels are stored.
	QString userDataDir;		// Where the user levels are stored.

	KMenuBar   *menuBar;

	QPopupMenu *file_menu;
	QPopupMenu *edit_menu;
	QPopupMenu *opt_menu;

	KStatusBar *statusBar;
	KToolBar   *toolBar;
	KIconLoader *loader;
	QPushButton *pauseBtn;

	bool exitWarning;
	QString about;

signals:
	void userGuideRequested();

private slots:
        void myQuit();				// Menu "Quit" or window-close.

/******************************************************************************/
/**************************  PLAYFIELD AND GAME DATA  *************************/
/******************************************************************************/

private:
	KGoldrunnerWidget *view;		// Widget to hold playfield.

	QTimer * mouseSampler;			// Timer for mouse tracking.

	KGrObj *playfield[30][22];		// Array of playfield objects.
	char	editObjArray[30][22];		// Character-code equivalent.
	char	lastSaveArray[30][22];		// Copy for use in "saveOK()".

	KGrHero *hero;				// The HERO figure !!!
	int startI, startJ;			// The hero's starting position.
	QList<KGrEnemy> enemies;		// The list of enemies.
	int enemyCount;				// How many enemies.
	KGrEnemy *enemy;			// One of the enemies.

	int nuggets;				// How many nuggets.

	QList<KGrCollection>	collections;	// List of ALL collections.
	KGrCollection *		collection;	// Collection currently in use.
	Owner			owner;		// Collection owner.
	int			collnOffset;	// Owner's index in collections.
	int			collnIndex;	// Index within owner.

	int	level;				// Current play/edit level.
	QString	levelName;			// Level name (optional).
	QString	levelHint;			// Level hint (optional).

	long	lifes;				// Lives remaining.
	long	score;				// Current score.

public:
	QString	getTitle();			// Collection - Level NNN, Name.

/******************************************************************************/
/**************************  GAME CONTROL PROCEDURES  *************************/
/******************************************************************************/

protected:
	void keyPressEvent (QKeyEvent *);

private:
	void setBlankLevel (bool);
	void startLevel (int action);
	void newGame (int);
	void startTutorial();
	void showHint();

        bool newLevel;			// T = new level, F = reloading current.
        bool loading;			// Inhibits input until level is loaded.
	int loadLevel (int);		// Loads from a file in a collection.
	bool openLevelFile (int, QFile &);
	void changeObject (unsigned char, int, int);
	void createObject (KGrObj*, int, int);
	void setTimings();
	void initSearchMatrix();

	void startPlaying();		// Set the hero and enemies moving.
	void saveGame();		// Save game ID, score and level.
	void loadGame();		// Re-load game, score and level.
	int  lgHighlight;		// Row selected in loadGame QListBox.
	void checkHighScore();		// Check if high score for current game.
	void showHighScores();		// Show high scores for current game.

	void freeze();			// Stop the game.
	void unfreeze();		// Restart the game.

	QString getFilePath (Owner, KGrCollection *, int);

	void initStatusBar();		// Status bar control.
	void changeLevel (int);
	void changeLifes (int);
	void changeScore (int);

private slots:
        void commandCallback(int);	// Menu actions.
        void rCheck(int);		// Set/unset ticks on rule menu-items.
        void sCheck(int);		// Set/unset ticks on speed menu-items.
	void pauseResume();		// Stop/restart play.

	void incScore(int);		// Update the score.
	void showHidden();		// Show hidden ladders (nuggets gone).
	void loseNugget();		// Nugget destroyed (not collected).
	void herosDead();		// Hero caught or killed (key K).
	void nextLevel();		// Hero completed the level.

	void readMousePos ();		// Timed reading of mouse position.
	void doDig(int);		// For mouse-click when in play-mode.

	void lgSelect(int);		// User selected a saved game to load.

/******************************************************************************/
/**************************  KGOLDRUNNER PAINT EVENT **************************/
/******************************************************************************/

protected:
	void paintEvent( QPaintEvent * );

/******************************************************************************/
/**************************  AUTHORS' DEBUGGING AIDS **************************/
/******************************************************************************/

private:
	void doStep();			// Do one animation step.
	void showFigurePositions();	// Show everybody's co-ordinates.
	void showHeroState();		// Show hero's co-ordinates and state.
	void showEnemyState (int);	// Show enemy's co-ordinates and state.
	void showObjectState();		// Show an object's state.

/******************************************************************************/
/********************  GAME EDITOR PROPERTIES AND METHODS  ********************/
/******************************************************************************/

private:
	bool editMode;		// Flag to change keyboard and mouse behaviour.
	char editObj;		// Type of object to be painted by the mouse.
	bool paintEditObj;	// Sets painting on/off (toggled by clicking).
	int  oldI, oldJ;	// Last mouse position painted.
	int  editLevel;		// Level to be edited (= 0 for new level).
	int  heroCount;		// Can enter at most one hero.

private slots:			// Slots connected to the Menu and Edit Toolbar.
	void createLevel();	// Set up a blank level-display for edit.
	void updateLevel();	// Select and load an existing level for edit.
	bool saveLevelFile();	// Save the edited level in a text file (.grl).

private:
	void loadEditLevel(int);// Load and display an existing level for edit.
	void moveLevelFile();	// Move level to another collection or number.
	void deleteLevelFile();	// Delete a level file.
	bool saveOK ();		// Check if changes have been saved.
	void initEdit();
	void deleteLevel();
	void insertEditObj (int, int);
	void showEditLevel();
	bool reNumberLevels (int, int, int, int);

	void makeEditToolbar();
	KToolBar    *editToolbar;
	int         pressedButton;

	// Pixmaps for repainting objects as they are edited.
	QPixmap digpix[10];
	QPixmap brickbg, fbrickbg;
	QPixmap freebg, nuggetbg, polebg, betonbg, ladderbg, hladderbg;
	QPixmap edherobg, edenemybg;

private slots:
	void doEdit(int);		// For mouse-click when in edit-mode.
	void endEdit(int);		// For mouse-release when in edit-mode.

/******************************************************************************/
/*************************   COLLECTIONS HANDLING   ***************************/
/******************************************************************************/

private:
	void mapCollections ();		// Check collections vs. ".grl" files.
	bool loadCollections (Owner);	// Load "collection.dat" file.
	bool saveCollections (Owner);	// Save "collection.dat" file.

/******************************************************************************/
/**********************    LEVEL SELECTION DIALOG BOX    **********************/
/******************************************************************************/

private:
	int selectLevel (int);		// Select collection and level.

	bool		modalFreeze;	// True when game is frozen temporarily.

	QDialog		*sl;		// Popup dialog box.
	QRadioButton	*systemB;	// Selects "System" collections.
	QRadioButton	*userB;		// Selects "User" collections.
	QListBox	*colln;		// Selects collection from a list.
	QLabel		*collnD;	// Describes collection selected.
	QScrollBar	*number;	// Slider to select level number.
	QLineEdit	*display;	// Text-entry version of level number.
	KGrThumbNail	*thumbNail;	// Preview of level selected.
	QLabel		*slName;	// Name of level selected.

	int		slAction;	// Current editing/play action.
	Owner		slOwner;	// Owner of selected collections.
	int		slCollnOffset;	// Index of that owner's collections.
	int		slCollnIndex;	// Index of selected collection.

	bool ownerOK(Owner);		// Check that the owner owns something.
	void slSetOwner(Owner);
	void slSetCollections(int);

private slots:
	void slSystemOwner();
	void slUserOwner();
	void slColln(int);
	void slAboutColln();
	void slShowLevel(int);
// The MOC compiler "simply skips any preprocessor directives it encounters"
// (see the QT documentation, "Using the MetaObject Compiler", "Limitations"
// section).  The following lines are commented/uncommented by the "fix_src"
// script, invoked during the "make init" step of installation.

	void slUpdate (const char *);			// QT1_ONLY
	void slPaintLevel();
	void slHelp();
	void slNameAndHint();

/******************************************************************************/
/**********************    COLLECTION EDIT DIALOG BOX    **********************/
/******************************************************************************/

private:
	void editCollection (int);	// Edit/Create details of a collection.
	QRadioButton	*ecKGrB;
	QRadioButton	*ecTradB;
	void ecSetRules (const char);

private slots:
	void ecSetKGr ();
	void ecSetTrad ();


/******************************************************************************/
/**********************    WORD-WRAPPED MESSAGE BOX    ************************/
/******************************************************************************/

private:
	void myMessage (QWidget *, QString, QString);
	bool	messageFreeze;		// True when game is frozen temporarily.
	void wordWrap (QMultiLineEdit *, int);	// NOT needed with QT 2.
};

#endif // KGOLDRUNNER_H
