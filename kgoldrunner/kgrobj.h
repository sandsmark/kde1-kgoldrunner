/***************************************************************************
                          kgrobj.h  -  description
                             -------------------
    begin                : Wed Jan 23 2002
    copyright            : (C) 2002 by Marco Krüger and Ian Wadham
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

/*  _                   _     _   _
 * | | ____ _ _ __ ___ | |__ (_) | |__
 * | |/ / _` | '__/ _ \| '_ \| | | '_ \
 * |   < (_| | | | (_) | |_) | |_| | | |
 * |_|\_\__, |_|  \___/|_.__// (_)_| |_|
 *      |___/ by grisu (99) |__/
 */

#ifndef KGROBJ__H
#define KGROBJ__H

// Enable the next "#define" or use compiler option -DQT1 if you want to compile
// with Qt Library v1.x and KDE v1.x desktop.  Otherwise, we will use Qt v2.x
// and no particular desktop (thus moving towards independence of O/S).
//
// #define QT1

#ifdef QT1
#include <kiconloader.h>
#endif

#include <iostream.h>

#include <qimage.h>
#include <qlist.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qwidget.h>
#include <stdlib.h> // für Zufallsfunktionen

const char FREE    = ' ';
const char ENEMY   = 'E';
const char HERO    = 'R';
const char BETON   = 'X';
const char BRICK   = 'M';
const char FBRICK  = 'F';
const char HLADDER = 'Z';
const char LADDER  = 'H';
const char NUGGET  = 'N';
const char POLE    = 'T';
const char HOLE    = 'O';
const char USEDHOLE= 'U';

const char CANWALKLEFT  = 0x1;
const char CANWALKRIGHT = 0x2;
const char CANWALKUP    = 0x4;
const char CANWALKDOWN  = 0x8;
const char VISITED      = 0x10;

const char FIELDWIDTH   = 28;
const char FIELDHEIGHT    = 20;

const char VERTIKAL     = 0;
const char HORIZONTAL   = 1;

/* Action times ... */
#define	NSPEED		12
#define	MAXSPEED	NSPEED * 2
#define	MINSPEED	NSPEED / 4

#define	BEGINSPEED	NSPEED / 2
#define	NOVICESPEED	(3 * NSPEED) / 4
#define	CHAMPSPEED	(3 * NSPEED) / 2

typedef struct {
    int hwalk;
    int hfall;
    int ewalk;
    int efall;
    int ecaptive;
    int hole;
} Timing;

const int DIGDELAY = 200;

const int STEP = 4;

const double DROPNUGGETDELAY = 70.0;	// Enemy holds gold for avg. 12.5 cells.
// const double DROPNUGGETDELAY = 500.0;// Old value --> avg. 94 cells.

enum Position {RIGHTWALK1,RIGHTWALK2,RIGHTWALK3,RIGHTWALK4,
	       LEFTWALK1,LEFTWALK2,LEFTWALK3,LEFTWALK4,
	       RIGHTCLIMB1,RIGHTCLIMB2,RIGHTCLIMB3,RIGHTCLIMB4,
	       LEFTCLIMB1,LEFTCLIMB2,LEFTCLIMB3,LEFTCLIMB4,
	       CLIMB1,CLIMB2,
	       FALL1,FALL2};
enum Status {STANDING,FALLING,WALKING,CLIMBING,CAPTIVE};
enum Direction {RIGHT,LEFT,UP,DOWN,STAND};
enum SearchStrategy {LOW,MEDIUM,HIGH};

class KGrFigure;

class KGrObj : public QWidget
{
  Q_OBJECT
 public:
  KGrObj (QWidget *parent=0, const char *name=0);
  virtual ~KGrObj();

  // STATIC GLOBAL FLAGS.
  static bool frozen;		// Game play halted (use ESCAPE key).
  static bool bugFixed;		// Dynamic bug fix turned on (key B, if halted).
  static bool logging;		// Log printing turned on.

  void drawFigure(QPixmap,int,int);
  void eraseFigure(int,int);
  void setx(int);
  void sety(int);
  char whatIam();
  int searchValue;
  bool blocker; // Beton or Brick -> TRUE
  void setMousePos ();
  void showState (int, int);
 signals:
  void mouseClick (int);
  void mouseLetGo (int);
 protected:
  void mousePressEvent (QMouseEvent *);
  void mouseReleaseEvent (QMouseEvent *);
  int xpos;
  int ypos;
  char iamA;
 private:
  QCursor * m;
};

class KGrEditable : public KGrObj
{
  Q_OBJECT
public:
  KGrEditable (const QPixmap & pic, QWidget *parent=0, const char *name=0);
  virtual ~KGrEditable ();
  void setType (char, const QPixmap &);
  void paintEvent (QPaintEvent *);
private:
  QPixmap bgPixmap;
};

class KGrFree : public KGrObj
{ Q_OBJECT
 public:
  KGrFree(const QPixmap & pic1, const QPixmap & pic2, bool = true,
			QWidget *parent = 0, const char *name = 0);
  virtual ~KGrFree();
  void setNugget(bool);
  void redrawFigure (KGrFigure *);
 protected:
  QPixmap freebg,nuggetbg;
  char theRealMe;	// Set to FREE or HLADDER, even when "iamA == NUGGET".
  KGrFigure * figure;
  void paintEvent (QPaintEvent *);
};

class KGrPole : public KGrObj
{ Q_OBJECT
 public:
  KGrPole(const QPixmap & pic, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrPole();
};

class KGrStones : public KGrObj
{ Q_OBJECT
 public:
  KGrStones(QWidget *parent = 0, const char *name = 0);
  virtual ~KGrStones();
};

class KGrBrick : public KGrStones
{ Q_OBJECT
 public:
  KGrBrick(const QPixmap * pic, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrBrick();
  void dig(void);
  void useHole();
  void unUseHole();
  static int speed;	// Digging & repair speed (copy of KGrFigure::speed).
  static int HOLETIME;	// Number of timing cycles for a hole to remain open.
  void doStep();
  void showState (int, int);
 protected slots:
  void timeDone(void);
 private :
  int dig_counter;
  int hole_counter;
  const QPixmap *digpix;
  bool holeFrozen;
  QTimer *timer;
};

class KGrBeton : public KGrStones
{ Q_OBJECT
 public:
  KGrBeton(const QPixmap & pic, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrBeton();
};

class KGrFbrick : public KGrStones
{ Q_OBJECT
 public:
  KGrFbrick(const QPixmap & pic, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrFbrick();
};

class KGrLadder : public KGrObj
{ Q_OBJECT
 public:
  KGrLadder(const QPixmap & pic, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrLadder();
};

class KGrHladder : public KGrFree
{ Q_OBJECT
 public:
  // BUG FIX - Ian W., 21/6/01 - must inherit "setNugget()" from "KGrFree".
  KGrHladder(const QPixmap & pic1, const QPixmap & pic2, const QPixmap & pic3,
		bool nug = 0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrHladder();
  void showLadder();
 protected:
  QPixmap ladderbg;
};

/*
 *      __ _
 *     / _(_) __ _ _   _ _ __ ___
 *    | |_| |/ _` | | | | '__/ _ \
 *    |  _| | (_| | |_| | | |  __/
 *    |_| |_|\__, |\__,_|_|  \___|
 *           |___/
 */

class KGrFigure : public QObject
{  Q_OBJECT

 public:
  KGrFigure (int, int);
  virtual ~KGrFigure();

  // STATIC GLOBAL FLAGS.
  static bool variableTiming;		// More enemies imply less speed.
  static bool alwaysCollectNugget;	// Enemies always collect nuggets.
  static bool runThruHole;		// Enemy can run L/R through dug hole.
  static bool reappearAtTop;		// Enemies are reborn at top of screen.
  static SearchStrategy searchStrategy;	// Low, medium or high difficulty.

  static Timing fixedTiming;		// Original set of 6 KGr timing values.

  static Timing varTiming [6];		// Optional 6 sets of timing values,
					// dependent on number of enemies.
  void incnuggetd();
  int getx();
  int gety();
  int getnuggets();
  void setNuggets(int n);
  void setPlayfield(KGrObj *(*p)[30][22]);
  void setx(int x);
  void sety(int y);
  void showFigure(); //zeigt Figur
  virtual void init(int,int);
  void eraseOldFigure();

 protected:

  // STATIC GLOBAL VARIABLES.
  static int herox;
  static int heroy;

  static int speed;		// Adjustable game-speed factor.

  int x, y;
  int absx, absy;
  int relx, rely; // Figur wird um relx,rely Pixel verschoben
  int mem_x,mem_y,mem_relx,mem_rely;
  int walkCounter;
  int nuggets;
  int actualPixmap; // ArrayPos der zu zeichnenden Pixmap
  QTimer *walkTimer;
  QTimer *fallTimer;

  KGrObj *(*playfield)[30][22];
  Status status;
  Direction direction;
  bool canWalkRight();
  bool canWalkLeft();
  virtual bool canWalkUp();
  bool canWalkDown();
  bool canStand();
  bool hangAtPole();
  virtual bool standOnEnemy()=0;
  void walkUp(int);
  void walkDown(int, int);
  void walkRight(int, int);
  void walkLeft(int, int);
  void initFall(int, int);

  bool walkFrozen;
  bool fallFrozen;
};

class KGrEnemy : public KGrFigure
{ Q_OBJECT

 public:
  KGrEnemy (const QPixmap & pic1, const QPixmap & pic2, int , int);
  virtual ~KGrEnemy();
  void showFigure();
  void startSearching();
  void setEnemyList(QList<KGrEnemy> *);
  virtual void init(int,int);
  static int WALKDELAY;
  static int FALLDELAY;
  static int CAPTIVEDELAY;
  int enemyId;
  void doStep();
  void showState (char);
 private:
  int birthX, birthY;
  int searchStatus;
  int captiveCounter;
  QTimer *captiveTimer;
  QPixmap enemyPm[2][20];
  bool canWalkUp();
  QList<KGrEnemy> *enemies;
  bool standOnEnemy();
  bool bumpingFriend();

  void startWalk();
  void dieAndReappear();
  Direction searchbestway(int,int,int,int);
  Direction searchdownway(int,int);
  Direction searchupway(int,int);
  Direction searchleftway(int,int);
  Direction searchrightway(int,int);

  Direction lowSearchUp(int,int,int);
  Direction lowSearchDown(int,int,int);
  Direction lowGetHero(int,int,int);

  int  distanceUp (int, int, int);
  int  distanceDown (int, int, int);
  bool searchOK (int, int, int);
  int  canWalkLR (int, int, int);
  bool willNotFall (int, int);

  void collectNugget();
  void dropNugget();

  bool captiveFrozen;

 public slots:
  void walkTimeDone();
  void fallTimeDone();
  void captiveTimeDone();

 signals:
  void lostNugget();
  void trapped(int);
  void killed(int);
};

class KGrHero : public KGrFigure
{  Q_OBJECT

 public:
  KGrHero(const QPixmap & pic, int , int);
  virtual ~KGrHero();
  bool started;
  void showFigure();
  void dig();
  void digLeft();
  void digRight();
  void startWalk();
  void setEnemyList(QList<KGrEnemy> *);
  void init(int,int);
  void setKey(Direction);
  void setDirection(int, int);
  void start();
  void loseNugget();
  static int WALKDELAY;
  static int FALLDELAY;
  void setSpeed(int);
  void doStep();
  void showState (char);
  QPixmap heroPm[20];
 private:
  QList<KGrEnemy> *enemies;
  // QPixmap heroPm[20];
  bool standOnEnemy();
  bool canWalkRight();
  bool canWalkLeft();
  bool canStay();
  bool isInEnemy();
  bool isInside(int, int);

  Direction nextDir;
  void collectNugget();

  int mousex;
  int mousey;
  void setNextDir();

 public slots:
  void walkTimeDone();
  void fallTimeDone();

 signals:
  void gotNugget(int);
  void haveAllNuggets();
  void leaveLevel();
  void caughtHero();

};

#endif
