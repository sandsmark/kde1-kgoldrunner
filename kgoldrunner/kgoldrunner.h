#ifndef KGOLDRUNNER_H 
#define KGOLDRUNNER_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <iostream.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kgrobj.h>
#include <ktopwidget.h>
#include <qcolor.h>
#include <qkeycode.h>
#include <qlist.h>
#include <qstring.h>


#include "kgoldrunnerwidget.h"

// KONSTANTEN

// DateiMenue
const int ID_NEW =  101;
const int ID_OPEN = 102;
const int ID_SAVE = 103;
const int ID_EXIT = 104;

// HilfeMenue
const int ID_HELP = 201;

// Statusbar
const int ID_LIFES      = 901;
const int ID_SCORE      = 902;
const int ID_LEVEL      = 903;
const int ID_LEVELPLACE = 904;
const int ID_DUMMY      = 905;

class KGoldrunner : public KTopLevelWidget
{
	Q_OBJECT
public:
	KGoldrunner(QWidget *parent = 0, const char *name = 0);
	virtual ~KGoldrunner();
	void keyPressEvent(QKeyEvent *);
		
protected slots:
	  //	void invokeHelp();
        void commandCallback(int);
	void incScore();
	void showHidden();
	void herosDead();
	void nextLevel();

private:
	KMenuBar   *menuBar;
	KStatusBar *statusBar;
	KToolBar   *toolBar;
	KGoldrunnerWidget *view;

	KGrObj *playfield[30][22];

	KIconLoader *loader;

	int level;
	int lifes;
	int score;
	int nuggets;
	int enemyCount;

	void changeObject(unsigned char, int, int);
	void deletelevel();
	void initStatusBar();
	void initSearchMatrix();
	int loadLevel(int);
	void newGame();
	void showlevel();
        bool newLevel;
	
	void changeLifes(int);
	void changeScore(int);
	void changeLevel(int);

	KGrHero *hero;
	KGrEnemy *enemy;
	QList<KGrEnemy> enemies;

	void createObject(KGrObj*, int, int);
 protected:
	void paintEvent( QPaintEvent * );
	
};

#endif // KGOLDRUNNER_H 
