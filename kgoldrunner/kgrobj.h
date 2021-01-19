/*  _                   _     _   _     
 * | | ____ _ _ __ ___ | |__ (_) | |__  
 * | |/ / _` | '__/ _ \| '_ \| | | '_ \ 
 * |   < (_| | | | (_) | |_) | |_| | | |
 * |_|\_\__, |_|  \___/|_.__// (_)_| |_|
 *      |___/ by grisu (99) |__/         
 */

#ifndef KGROBJ__H
#define KGROBJ__H

#include <iostream.h>

#include <kapp.h>
#include <kiconloader.h>
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

const double DROPNUGGETDELAY = 500.0;
const int DIGDELAY = 200;
const int HOLETIME = 40;
const int STEP = 4;

enum Position {RIGHTWALK1,RIGHTWALK2,RIGHTWALK3,RIGHTWALK4,
	       LEFTWALK1,LEFTWALK2,LEFTWALK3,LEFTWALK4,
	       RIGHTCLIMB1,RIGHTCLIMB2,RIGHTCLIMB3,RIGHTCLIMB4,
	       LEFTCLIMB1,LEFTCLIMB2,LEFTCLIMB3,LEFTCLIMB4,
	       CLIMB1,CLIMB2,
	       FALL1,FALL2};
enum Status {STANDING,FALLING,WALKING,CLIMBING,CAPTIVE};
enum Direction {RIGHT,LEFT,UP,DOWN,STAND};

class KGrObj : public QWidget
{ 
  Q_OBJECT
 public:
  KGrObj(KIconLoader *loader=0, QWidget *parent=0, const char *name=0);
  virtual ~KGrObj();
  void drawFigure(QPixmap,int,int);
  void eraseFigure(int,int);
  void setx(int);
  void sety(int);
  char whatIam();
  int searchValue;
  bool blocker; // Beton or Brick -> TRUE
 protected:
  int xpos;
  int ypos;
  char iamA;
  KIconLoader *loader;
  QPainter painter;
};

class KGrFree : public KGrObj
{ Q_OBJECT
 public:
  KGrFree(KIconLoader *loader=0, bool = true, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrFree();
  void setNugget(bool);
 protected:
  QPixmap freebg,nuggetbg;
};

class KGrPole : public KGrObj
{ Q_OBJECT
 public:
  KGrPole(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrPole();
};

class KGrStones : public KGrObj
{ Q_OBJECT
 public:
  KGrStones(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrStones();
};

class KGrBrick : public KGrStones
{ Q_OBJECT
 public:
  KGrBrick(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrBrick();
  void dig(void);
  void useHole();
  void unUseHole();
 protected slots:
  void timeDone(void);
 private :
  int dig_counter;
  QPixmap digpix[10];
  QTimer *timer;
};

class KGrBeton : public KGrStones
{ Q_OBJECT
 public:
  KGrBeton(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrBeton();
};

class KGrFbrick : public KGrStones
{ Q_OBJECT
 public:
  KGrFbrick(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrFbrick();
};

class KGrLadder : public KGrObj
{ Q_OBJECT
 public:
  KGrLadder(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
  virtual ~KGrLadder();
};

class KGrHladder : public KGrLadder
{ Q_OBJECT
 public:
  KGrHladder(KIconLoader *loader=0, QWidget *parent = 0, const char *name = 0);
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
  KGrFigure(KIconLoader *, int , int );
  virtual ~KGrFigure();
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

 protected:
  static int herox;
  static int heroy;
  int x, y;
  int absx, absy;
  int relx, rely; // Figur wird um relx,rely Pixel verschoben 
  int mem_x,mem_y,mem_relx,mem_rely;
  int walkCounter;
  int nuggets;
  int actualPixmap; // ArrayPos der zu zeichnenden Pixmap
  int WALKDELAY;
  int FALLDELAY;
  QTimer *walkTimer;
  QTimer *fallTimer;

  KGrObj *(*playfield)[30][22];
  KIconLoader *loader;
  Status status;
  Direction direction;
  bool canWalkRight();
  bool canWalkLeft();
  virtual bool canWalkUp();
  bool canWalkDown();
  bool canStand();
  bool hangAtPole();
  virtual bool standOnEnemy()=0;
  void walkUp();
  void walkDown();
  void walkRight();
  void walkLeft();
  void initFall(int);
  void eraseOldFigure();
};

class KGrEnemy : public KGrFigure
{ Q_OBJECT 

 public:
  KGrEnemy(KIconLoader *, int , int );
  virtual ~KGrEnemy();
  void showFigure();
  void startSearching();
  void setEnemyList(QList<KGrEnemy> *);
  virtual void init(int,int);
  int enemyId;
 private:
  int birthX, birthY;
  int CAPTIVEDELAY;
  int searchStatus;
  int captiveCounter;
  bool heroOnMyHead;
  QTimer *captiveTimer;
  QPixmap enemyPm[2][20];
  bool canWalkUp();
  QList<KGrEnemy> *enemies;
  bool standOnEnemy();
  bool isInside(int,int);
  bool isInEnemy();

  Direction searchbestway(int,int,int,int);
  Direction searchdownway(int,int,int,int);
  Direction searchupway(int,int,int,int);
  Direction searchleftway(int,int,int,int);
  Direction searchrightway(int,int,int,int);
  void collectNugget();
  void dropNugget();

 public slots:
  void walkTimeDone();
  void fallTimeDone();
  void captiveTimeDone();
  
 signals:
};

class KGrHero : public KGrFigure
{  Q_OBJECT 

 public:
  KGrHero(KIconLoader *, int , int );
  virtual ~KGrHero();
  bool started;
  void showFigure(); 
  void dig();
  void startWalk();
  void setEnemyList(QList<KGrEnemy> *);
  void init(int,int);
  void setKey(Direction);
  void start();
 private:
  QList<KGrEnemy> *enemies;
  QPixmap heroPm[20];
  bool standOnEnemy();
  bool canWalkRight();
  bool canWalkLeft();
  bool canStay();
  bool isInEnemy();
  bool isInside(int, int);

  Direction nextDir;
  void collectNugget();

 public slots:
  void walkTimeDone();
  void fallTimeDone();

 signals:
  void gotNugget();
  void haveAllNuggets();
  void leaveLevel();
  void caughtHero();
  
};

#endif
