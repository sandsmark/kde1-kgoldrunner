/***************************************************************************
                          kgrobj.cpp  -  description
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

// #include <qwmatrix.h>
#include "kgrobj.h"
#include "kgrobj.moc"

#include <stdio.h>

KGrObj :: KGrObj (QWidget * parent, const char *name)
  :QWidget (parent, name)
{
  resize (16, 16);
  setFixedSize (16, 16);
  blocker = false;
}

bool KGrObj::frozen = FALSE;	// Initialise game as running, not halted.
bool KGrObj::bugFixed = FALSE;	// Initialise game with dynamic bug-fix OFF.
bool KGrObj::logging = FALSE;	// Initialise game with log printing OFF.

void KGrObj::mousePressEvent (QMouseEvent * m) {
    emit mouseClick (m->button ());
}

void KGrObj::mouseReleaseEvent (QMouseEvent * m) {
    emit mouseLetGo (m->button ());
}

void KGrObj::eraseFigure (int x, int y)
{
  erase (x, y, 16, 16);
}

void KGrObj::drawFigure (QPixmap f, int x, int y)
{
  QPainter painter(this);
  painter.drawPixmap (x, y, f);
  painter.end();
}

void KGrObj::setx (int x)
{
  xpos = x;
}

void KGrObj::sety (int y)
{
  ypos = y;
}

char KGrObj::whatIam ()
{
  return iamA;
}

void KGrObj::setMousePos ()
{
  m->setPos (mapToGlobal (QPoint (8,8)));
}

void KGrObj::showState (int i, int j)
{
  printf("(%02d,%02d) - Object [%c] search %d", i, j, iamA, searchValue);
  if (blocker) printf(" blocker");
  printf("\n");
}

KGrObj :: ~KGrObj ()
{
}


KGrEditable::KGrEditable (const QPixmap &pic, QWidget *parent, const char *name)
    :KGrObj (parent, name)
{
    setBackgroundMode (NoBackground);
    bgPixmap = pic;
    iamA = FREE;
}

void KGrEditable::setType (char editType, const QPixmap & editPixmap)
{
//    Sample code for resizing objects.  Must also disable or rewrite
//    the "setFixedSize(16,16);" invocation in the "KGrObj" code.  Maybe
//    use "setMaximumSize(32,32);" and "setMinimumSize(16,16);".
//    if (editType == LADDER) {
//	QWMatrix wm;
//	wm.scale (2.0, 2.0);
//	resize (32,32);
//	raise();
//	editPixmap = editPixmap.xForm(wm);
//    }
    bgPixmap = editPixmap;
    repaint();
    iamA = editType;
}

KGrEditable::~KGrEditable ()
{
}

void KGrEditable::paintEvent (QPaintEvent * ev)
{
    if (ev->rect().width() > 0)	// Use "ev": just to avoid compiler warnings.
	;

    QPainter painter(this);
    painter.drawPixmap (0, 0, bgPixmap);
    painter.flush();
    painter.end();
}

KGrFree :: KGrFree (const QPixmap & pic1, const QPixmap & pic2, bool nug,
				  QWidget * parent, const char *name)
  :KGrObj (parent, name)
{
  figure	= NULL;
  freebg	= pic1;
  nuggetbg	= pic2;

  theRealMe	= FREE;		// Remember what we are, even when "nug==TRUE".
  setNugget(nug);
}

void KGrFree::setNugget(bool nug)
{
    // This code must work over a hidden ladder as well as a free cell.
    if (! nug) {
	setBackgroundPixmap (freebg);
	iamA = theRealMe;
    }
    else {
	setBackgroundPixmap (nuggetbg);
	iamA = NUGGET;
    }
}

void KGrFree::redrawFigure (KGrFigure * f)
{
    figure = f;			// Set figure for paintEvent to draw later.
}

void KGrFree :: paintEvent (QPaintEvent * ev)
{
    if (ev->rect().width() > 0)	// Use "ev": just to avoid compiler warnings.
	;

    if (figure != NULL) {
	// Redraw hero or enemy after he has collected or dropped a nugget.
	// Needed because QT 2 re-draws the background pixmap AFTER the
	// showFigure() in the timer event and makes the hero/enemy disappear!
	if (figure->isA ("KGrHero"))
	    ((KGrHero *) figure)->showFigure();
	else
	    ((KGrEnemy *) figure)->showFigure();
	figure = NULL;
    }
}

KGrFree :: ~KGrFree ()
{
}

KGrPole :: KGrPole (const QPixmap & pic, QWidget * parent, const char *name)
  :KGrObj (parent, name)
{
  setBackgroundPixmap (pic);
  iamA = POLE;
}

KGrPole :: ~KGrPole ()
{
}

KGrStones :: KGrStones (QWidget * parent, const char *name)
  :KGrObj (parent, name)
{
  blocker = true;
}

KGrStones :: ~KGrStones ()
{
}

/* +++++++++++++++ BRICK ++++++++++++++++ */

KGrBrick :: KGrBrick (const QPixmap *pic, QWidget * parent, const char *name)
  :KGrStones (parent, name)
{
  digpix = pic;
  setBackgroundPixmap (digpix[0]);
  timer = new QTimer (this);
  connect (timer, SIGNAL (timeout ()), SLOT (timeDone ()));
  dig_counter = 0;
  holeFrozen = FALSE;
  iamA = BRICK;
}

void KGrBrick::dig (void)
{
  dig_counter = 1;
  hole_counter = HOLETIME;
  iamA = HOLE;
  setBackgroundPixmap (digpix[1]);
  timer->start ((DIGDELAY * NSPEED) / speed, TRUE);
}

void KGrBrick::doStep() {
    if (holeFrozen) {
	holeFrozen = FALSE;
	timeDone();
    }
}

void KGrBrick::showState (int i, int j)
{
    printf ("(%02d,%02d) - Brick  [%c] search %d dig-counter %d",
	i, j, iamA, searchValue, dig_counter);
    if (blocker)
	printf (" blocker");
    printf ("\n");
}

void KGrBrick::timeDone ()
{
    if (KGrObj::frozen) {holeFrozen = TRUE; return;}

    // When the hole is complete, we need a longer delay.
    if (dig_counter == 5) {
	hole_counter--;
	if (hole_counter > 0) {
	    timer->start ((DIGDELAY * NSPEED) / speed, TRUE);
	    return;
	}
    }
    if (dig_counter < 9) {
	dig_counter++;
	timer->start ((DIGDELAY * NSPEED) / speed, TRUE);
	if (dig_counter >= 8)
	    iamA = BRICK;
    }
    else
	dig_counter = 0;

    // Brick pix:- 0 normal, 1-4 crumbling, 5 hole complete, 6-9 re-growing.
    setBackgroundPixmap (digpix[dig_counter]);
}

void KGrBrick::useHole() {
    if (iamA == HOLE)
	iamA = USEDHOLE;
}

void KGrBrick::unUseHole() {
    if (iamA == USEDHOLE)
	iamA = HOLE;
}

KGrBrick :: ~KGrBrick ()
{
    delete timer;
}

KGrFbrick :: KGrFbrick (const QPixmap & pic , QWidget *parent, const char *name)
  :KGrStones (parent, name)
{
  setBackgroundPixmap (pic);
  iamA = FBRICK;
}

KGrFbrick :: ~KGrFbrick ()
{
}

KGrBeton :: KGrBeton (const QPixmap & pic, QWidget * parent, const char *name)
  :KGrStones (parent, name)
{
  setBackgroundPixmap (pic);
  iamA = BETON;
}

KGrBeton :: ~KGrBeton ()
{
}

KGrLadder :: KGrLadder (const QPixmap & pic, QWidget * parent, const char *name)
  :KGrObj (parent, name)
{
  setBackgroundPixmap (pic);
  iamA = LADDER;
}

KGrLadder :: ~KGrLadder ()
{
}

KGrHladder :: KGrHladder (const QPixmap & pic1, const QPixmap & pic2,
			const QPixmap & pic3, bool nug,
			QWidget * parent, const char *name)
  :KGrFree (pic1, pic2, nug, parent, name)
  // Must inherit "setNugget()" from "KGrFree".
{
  ladderbg = pic3;
  theRealMe = HLADDER;		// But remember we are a hidden ladder ...
  iamA = HLADDER;
}

void KGrHladder::showLadder()
{
  iamA = LADDER;
  setBackgroundPixmap (ladderbg);
}

KGrHladder :: ~KGrHladder ()
{
}
/*******************************************************
                          _____ _
                         |  ___(_) __ _ _   _ _ __ ___
                         | |_  | |/ _` | | | | '__/ _ \
                         |  _| | | (_| | |_| | | |  __/
                         |_|   |_|\__, |\__,_|_|  \___|
                                  |___/

********************************************************/
KGrFigure :: KGrFigure (int x, int y)
{
  this->x = mem_x = x;
  this->y = mem_y = y;
  relx = mem_relx = 0;
  rely = mem_rely = 0;

  absx = x*16;
  absy = y*16;

  nuggets = 0;
  status = STANDING;

  walkTimer = new QTimer (this);
  fallTimer = new QTimer (this);
}

// Initialise the global settings flags.
bool           KGrFigure::variableTiming = TRUE;
bool           KGrFigure::alwaysCollectNugget    = TRUE;
bool           KGrFigure::runThruHole    = TRUE;
bool           KGrFigure::reappearAtTop  = TRUE;
SearchStrategy KGrFigure::searchStrategy = LOW;

int KGrFigure::herox = 0;
int KGrFigure::heroy = 0;

// Initialise the global game-speed factors.
int KGrFigure::speed = NSPEED;
int KGrBrick::speed  = NSPEED;

// Initialise constants for fixed (KGoldrunner) and variable (Traditional)
// timing.  Each row contains timings for hero walk and fall, enemy walk and
// fall, enemy captured in hole and dug brick.

Timing KGrFigure::fixedTiming =	{45, 50, 55, 100, 500, 40};	// KGr original.

Timing KGrFigure::varTiming[6] = {				// Traditional.
				{40, 58, 78, 88, 170, 23},	// No enemies.
				{50, 68, 78, 88, 170, 32},	// 1 enemy.
				{57, 67, 114, 128, 270, 37},	// 2 enemies.
				{60, 70, 134, 136, 330, 40},	// 3 enemies.
				{63, 76, 165, 150, 400, 46},	// 4 enemies.
				{70, 80, 189, 165, 460, 51}	// >4 enemies.
};

int KGrBrick::HOLETIME = 0;

int KGrFigure::getx()
{
  return absx;
}

int KGrFigure::gety()
{
  return absy;
}

void KGrFigure::setx(int a)
{
  x=a;
}

void KGrFigure::sety(int b)
{
  y=b;
}

void KGrFigure::init(int a,int b)
{
  walkTimer->stop();
  fallTimer->stop();
  x = mem_x = a;
  y = mem_y = b;
  relx = mem_relx = 0;
  rely = mem_rely = 0;
  nuggets = 0;
  status = STANDING;
}

void KGrFigure:: setNuggets(int n)
{
  nuggets = n;
}


bool KGrFigure::canWalkRight()
{
  return (((*playfield)[x+1][y]->whatIam() != BRICK) &&
	  ((*playfield)[x+1][y]->whatIam() != BETON) &&
	  ((*playfield)[x+1][y]->whatIam() != FBRICK));
}

bool KGrFigure::canWalkLeft()
{
  return (((*playfield)[x-1][y]->whatIam() != BRICK) &&
	  ((*playfield)[x-1][y]->whatIam() != BETON) &&
	  ((*playfield)[x-1][y]->whatIam() != FBRICK));
	  }

bool KGrFigure::canWalkUp()
{
  return (((*playfield)[x][y-1]->whatIam() != BRICK) &&
	  ((*playfield)[x][y-1]->whatIam() != BETON) &&
	  ((*playfield)[x][y-1]->whatIam() != FBRICK) &&
	  ((*playfield)[x][y]->whatIam() == LADDER));
}

bool KGrFigure::canWalkDown()
{
  return (((*playfield)[x][y+1]->whatIam() != BRICK) &&
	  ((*playfield)[x][y+1]->whatIam() != BETON) &&
	  // v0.3 FIX - Let figure step down into FBRICK from a ladder.
	  //	  ((*playfield)[x][y+1]->whatIam() != FBRICK)&&
	  (((*playfield)[x][y+1]->whatIam() == LADDER)||
	   ((*playfield)[x][y]->whatIam() == LADDER)));
}

bool KGrFigure::canStand()
{
  return (((*playfield)[x][y+1]->whatIam() == BRICK) ||
	  ((*playfield)[x][y+1]->whatIam() == BETON) ||
	  ((*playfield)[x][y+1]->whatIam() == USEDHOLE)||
	  ((*playfield)[x][y+1]->whatIam() == LADDER)||
	  ((*playfield)[x][y]->whatIam() == LADDER)||
	  standOnEnemy());
	  }

bool KGrFigure::hangAtPole()
{
  return ((*playfield)[x][y]->whatIam() == POLE);
}

void KGrFigure::walkUp(int WALKDELAY)
{
    actualPixmap = (actualPixmap == CLIMB1) ? CLIMB2 : CLIMB1;
    if (walkCounter++ < 4) {
	// Not end of 4-step cycle: move one step up, if possible.
	if (canWalkUp()) {
	    rely -= STEP;
	    absy -= STEP;
	}
	walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
    }
    else {
	// End of 4-step cycle: move up to next cell, if possible.
	if (canWalkUp()) {
	    y--;
	}
	// Always reset position, in case we are stuck partly into a brick.
	rely = 0;
	absy = y*16;

	// Wait for caller to set next direction.
	status = STANDING;
    }
}

void KGrFigure::walkDown(int WALKDELAY, int FALLDELAY)
{
    if (hangAtPole() || (! canStand())) {
	// On bar or no firm ground underneath: so must fall.
	initFall (FALL2, FALLDELAY);
    }
    else {
	actualPixmap = (actualPixmap == CLIMB1) ? CLIMB2 : CLIMB1;
	if (walkCounter++ < 4) {
	    // Not end of 4-step cycle: move one step down, if possible.
	    if (canWalkDown()) {
		rely += STEP;
		absy += STEP;
	    }
	    walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
	}
	else {
	    // End of 4-step cycle: move down to next cell, if possible.
	    if (canWalkDown()) {
		y++;
	    }
	    // Always reset position, in case we are stuck partly into a brick.
	    rely = 0;
	    absy = y*16;

	    // Must be able to halt at a pole when going down.
	    if (! (canStand() || hangAtPole()))
		initFall(FALL2, FALLDELAY);	// kein Halt....urgs
	    else
		// Wait for caller to set next direction.
		status = STANDING;
	}
    }
}

void KGrFigure::walkLeft (int WALKDELAY, int FALLDELAY)
{
    // If counter != 0, the figure is walking, otherwise he is turning around.
    if (walkCounter++ != 0) {
	// Change to the next pixmap in the animation.
	if ((++actualPixmap%4) != 0) {
	    // Not end of 4-pixmap cycle: move one step left, if possible.
	    if (canWalkLeft()) {
		relx -= STEP;
		absx -=STEP;
	    }
	    walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
	}
	else {
	    // End of 4-pixmap cycle: start again, in next cell if possible.
	    actualPixmap -= 4;
	    if (canWalkLeft()) {
		x--;
	    }
	    // Always reset position, in case we are stuck partly into a brick.
	    relx = 0;
	    absx = x*16;

	    // If cannot stand or hang, start fall, else await next assignment.
	    if (! (canStand() || hangAtPole()))
		initFall (FALL1, FALLDELAY);
	    else
		status = STANDING;	// Caller should set next direction.
	}
    }
    else {
	status = STANDING;		// The figure is turning around.
    }
}

void KGrFigure::walkRight(int WALKDELAY, int FALLDELAY)
{
    if (walkCounter++) {		// wenn 0, soll sich Figur nur umdrehen
	if (++actualPixmap % 4) {	// wenn true, dann ist kein vollständiger Schritt gemacht
	    if (canWalkRight()) {
		relx += STEP;
		absx += STEP;	// nur vorwärts gehen, wenn es auch möglich ist
	    }
	    walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
	}
	else {
	    actualPixmap -= 4;		// Schritt war vollendet
	    if (canWalkRight()) {
		x++;
	    }				//gehe entgültig nach rechts
	    // Always reset position, in case we are stuck partly into a brick.
	    relx = 0;
	    absx = x*16;

	    if (!(canStand()||hangAtPole())) // kein Halt mehr...arrrgghhh
		initFall (FALL2, FALLDELAY);
	    else
		status = STANDING;	// Figur hat Halt
	}
    }
    else {
	status = STANDING;		// Figur sollte sich nur Umdrehen.
    }
}

void KGrFigure::eraseOldFigure ()
{
  (*playfield)[mem_x][mem_y]->eraseFigure (mem_relx, mem_rely);

  if (mem_relx != 0)
    if (mem_relx < 0)
      (*playfield)[mem_x-1][mem_y]->eraseFigure (mem_relx + 16, mem_rely);
    else
      (*playfield)[mem_x+1][mem_y]->eraseFigure (mem_relx - 16, mem_rely);
  else if (mem_rely != 0)
    if (mem_rely < 0)
      (*playfield)[mem_x][mem_y-1]->eraseFigure (mem_relx, mem_rely + 16);
    else
      (*playfield)[mem_x][mem_y+1]->eraseFigure (mem_relx, mem_rely - 16);
}

void KGrFigure::initFall(int apm, int FALLDELAY)
{
  status = FALLING;
  actualPixmap = apm;
  walkCounter=1;
  walkTimer->stop();
  fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
}

void KGrFigure::showFigure ()
{
}

void KGrFigure::setPlayfield (KGrObj * (*p)[30][22])
{
  playfield = p;
}

KGrFigure :: ~KGrFigure ()
{
}
/*******************************************************
                             _   _
                            | | | | ___ _ __ ___
                            | |_| |/ _ \ '__/ _ \
                            |  _  |  __/ | | (_) |
                            |_| |_|\___|_|  \___/

********************************************************/
KGrHero :: KGrHero (const QPixmap & pic, int x, int y)
  :KGrFigure (x, y)
{
  QPixmap pixmap;
  QImage image;

  status = STANDING;
  actualPixmap = 18;

  pixmap = pic;
  image = pixmap.convertToImage ();
  for (int i = 0; i < 20; i++)
    {
      heroPm[i].convertFromImage (image.copy (i * 16, 0, 16, 16));
    }

  herox=x;
  heroy=y;

  started = FALSE;
  walkCounter = 1;

  connect (walkTimer, SIGNAL (timeout ()), SLOT (walkTimeDone ()));
  connect (fallTimer, SIGNAL (timeout ()), SLOT (fallTimeDone ()));

  walkFrozen = FALSE;
  fallFrozen = FALSE;
}

int KGrHero::WALKDELAY = 0;
int KGrHero::FALLDELAY = 0;

/* Es ist Notwendig der eigentlichen Timerfunktion diese
   Startwalk vorzuschalten, um bei einem evtl. notwendigem
   Richtungswechsel der Figur diese Bewegung mit einzufügen */
void KGrHero::startWalk ()
{
  switch (nextDir) {
    case UP:
      if ((*playfield)[x][y]->whatIam () == LADDER)
	{walkCounter = 1;
	direction = UP;}
      break;
    case RIGHT:
      if (hangAtPole())
	actualPixmap = RIGHTCLIMB1;
      else
	actualPixmap = RIGHTWALK1;
      if (direction != RIGHT)
	walkCounter = 0;
      else
	walkCounter = 1;
      direction = RIGHT;
      break;
    case DOWN:
      if (((*playfield)[x][y]->whatIam () == LADDER)||
	  ((*playfield)[x][y+1]->whatIam () == LADDER))
	{walkCounter = 1;
	direction = DOWN;}
      else // wenn figur an Stange haengt und nichts unter ihr,
	if (hangAtPole() && (!canStand())) // dann soll sie fallen
	  { status = STANDING;
	  actualPixmap = (direction==RIGHT)?19:18;
	  walkCounter=1;
	  direction=STAND;
	  walkTimer->stop();
	  }
      break;
    case LEFT:
      if (hangAtPole())
	actualPixmap = LEFTCLIMB1;
      else
	actualPixmap = LEFTWALK1;
      if (direction != LEFT)
	walkCounter = 0;
      else
	walkCounter = 1;
      direction = LEFT;
      break;
    default :
      direction = STAND;
      status = FALLING;
      break;
    }
  nextDir = STAND;
  if (status != FALLING)//immer ausführen, ausser beim fallen
    { status = WALKING; // da sonst der FALLINGstatus wieder
    showFigure ();      // geaendert wird und der falsche Timer anspringt.
    }
} // END KGrHero::startWalk

void KGrHero::setKey(Direction key)
{
  nextDir = key;
}

void KGrHero::setDirection(int i, int j)
{
    mousex = i;
    mousey = j;
}

void KGrHero::setNextDir()
{
    int dx, dy;

    dx = mousex - x; dy = mousey - y;

    if ((dy == 0) && (y == 1) && (nuggets <= 0)) {
	nextDir = UP;
    }
    else if ((dy > 0) &&
	     (canWalkDown() ||
	      standOnEnemy() ||
	      (hangAtPole() && ((*playfield)[x][y+1]->whatIam() != BRICK) &&
			       ((*playfield)[x][y+1]->whatIam() != BETON)))) {
	nextDir = DOWN;
    }
    else if ((dy < 0) && canWalkUp ()) {
	nextDir = UP;
    }
    else if (dx > 0) {
	nextDir = RIGHT;
    }
    else if (dx < 0) {
	nextDir = LEFT;
    }
    else if (dx == 0) {
	nextDir = STAND;
    }
}

void KGrHero::doStep() {
    if (walkFrozen) {
	walkFrozen = FALSE;
	walkTimeDone();
    }
    if (fallFrozen) {
	fallFrozen = FALSE;
	fallTimeDone();
    }
}

void KGrHero::showState(char option)
{
  printf("(%02d,%02d) - Hero      ", x, y);
  switch (option) {
      case 'p': printf ("\n"); break;
      case 's': printf (" nuggets %02d status %d walk-ctr %d ",
			nuggets, status, walkCounter);
	    printf ("dirn %d next dirn %d\n", direction, nextDir);
	    printf ("                     rel (%02d,%02d) abs (%03d,%03d)",
			relx, rely, absx, absy);
	    printf (" pix %02d", actualPixmap);
	    printf (" mem %d %d %d %d", mem_x, mem_y, mem_relx, mem_rely);
	    if (walkFrozen) printf (" wBlock");
	    if (fallFrozen) printf (" fBlock");
	    printf ("\n");
	    break;
  }
}

void KGrHero::init(int a,int b)
{
    walkTimer->stop();
    fallTimer->stop();
    walkCounter = 1;
    started = FALSE;

    x = mem_x = a;
    y = mem_y = b;
    relx = mem_relx = 0;
    rely = mem_rely = 0;

    absx = 16*x;
    absy = 16*y;

    nuggets = 0;

    herox=x;
    heroy=y;

    actualPixmap=19;
}

void KGrHero::start()
{
    started = TRUE;
    walkFrozen = FALSE;
    fallFrozen = FALSE;

    if (!(canStand()||hangAtPole())) {		// Held muss wohl fallen...
	status = FALLING;
	fallTimeDone();
    }
    else {
	status = STANDING;
	walkTimeDone();
    }
}

void KGrHero::setSpeed (int gamespeed)
{
  if (gamespeed >= 0) {
    if (gamespeed < MINSPEED)
	speed++;		// Increase speed.
    else
	speed = gamespeed;	// Set selected speed.
    if (speed > MAXSPEED)
	speed = MAXSPEED;	// Set upper limit.
  }
  else {
    speed--;			// Reduce speed.
    if (speed < MINSPEED)
	speed = MINSPEED;	// Set lower limit.
  }

  KGrBrick::speed = speed;	// Make a copy for bricks.
}

void KGrHero::walkTimeDone ()
{
  if (! started) return;	// Ignore signals from earlier play.
  if (KGrObj::frozen) {walkFrozen = TRUE; return; }

  if ((*playfield)[x][y]->whatIam() == BRICK) {
    emit caughtHero();		// Brick closed over hero.
    return;
  }

  if ((y==1)&&(nuggets<=0)) {	// If on top row and all nuggets collected,
    emit leaveLevel();		// the hero has won and can go to next level.
    return;
  }

  if (status == STANDING)
    setNextDir();
  if ((status == STANDING) && (nextDir != STAND)) {
    if ((standOnEnemy()) && (nextDir == DOWN)) {
	emit caughtHero();	// Hero is going to step down into an enemy.
	return;
    }
    startWalk();
  }
  if (status != STANDING) {
      switch (direction) {
      case UP:		walkUp (WALKDELAY); break;
      case DOWN:	walkDown (WALKDELAY, FALLDELAY); break;
      case RIGHT:	walkRight (WALKDELAY, FALLDELAY); break;
      case LEFT:	walkLeft (WALKDELAY, FALLDELAY); break;
      default :
	// The following code is strange.  It makes the hero fall off a pole.
	// It works because of other strange code in "startWalk(), case DOWN:".
	if (!canStand()||hangAtPole()) // falling
	  initFall(FALL1, FALLDELAY);
	else  status = STANDING;
      break;
      }
    herox=x;heroy=y; // Koordinatenvariablen neu
    // wenn Held genau ein Feld weitergelaufen ist,
    if ((relx==0)&&(rely==0)) // dann setzte statische
      {
      collectNugget(); // und nehme evtl. Nugget
      }
    showFigure();
    }
  if (status == STANDING)
    if (!canStand()&&!hangAtPole())
      initFall(FALL1, FALLDELAY);
    else
      walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
  if(isInEnemy())
    emit caughtHero();
}

void KGrHero::fallTimeDone()
{
    if (! started) return;		// Ignore signals from earlier play.
    if (KGrObj::frozen) {fallFrozen = TRUE; return; }

    if (!standOnEnemy()) {
	if (walkCounter++ < 4) {	// Held fällt vier Positionen
	    fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
	    rely+=STEP;
	    absy+=STEP;
	}
	else {				//Held ist ein Feld weitergefallen
	    // Verschiebung der Figur zum 0-Punkt des Objekts (Brick etc...)
	    heroy = ++y;
	    rely = 0;
	    absy = y*16;		// wird Null und Figur eins runter
	    collectNugget();		// gesetzt. Zeit evtl. Nugget zu nehmen
	    if (! (canStand()||hangAtPole())) {	// Held muss wohl weiterfallen.
		fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
		walkCounter = 1;
	    }
	    else {			// Held hat Boden (oder Feind) unter den
		status = STANDING;	// Füssen oder hängt an Stange -> steh!
		walkTimer->start((WALKDELAY * NSPEED) / speed, TRUE);
		direction = (actualPixmap == 19) ? RIGHT : LEFT;
		if ((*playfield)[x][y]->whatIam() == POLE)
		    actualPixmap = (direction == RIGHT)? RIGHTCLIMB1:LEFTCLIMB1;
		else
		    actualPixmap = (direction == RIGHT)? RIGHTWALK1:LEFTWALK1;
	    }
	}
	showFigure();
    }
    else {
	if (rely == 0) {
	    // If at the bottom of a cell, try to walk or just stand still.
	    status = STANDING;
	    direction = (actualPixmap == 19) ? RIGHT : LEFT;
	    if ((*playfield)[x][y]->whatIam() == POLE)
		actualPixmap = (direction == RIGHT)? RIGHTCLIMB1:LEFTCLIMB1;
	    else
		actualPixmap = (direction == RIGHT)? RIGHTWALK1:LEFTWALK1;
	    walkTimer->start((WALKDELAY * NSPEED) / speed, TRUE);
	}
	else {
	    // Else, freeze hero until enemy moves out of the way.
	    fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
	}
    }
    if (isInEnemy() && (! standOnEnemy()))
	emit caughtHero();
}


void KGrHero::showFigure () {

  eraseOldFigure ();
  (*playfield)[x][y]->drawFigure (heroPm[actualPixmap], relx, rely);
  if (relx != 0)
    if (relx < 0)
      (*playfield)[x - 1][y]->drawFigure (heroPm[actualPixmap], relx + 16, rely);
    else
      (*playfield)[x + 1][y]->drawFigure (heroPm[actualPixmap], relx - 16, rely);
  else if (rely != 0)
    if (rely < 0)
      (*playfield)[x][y - 1]->drawFigure (heroPm[actualPixmap], relx, rely + 16);
    else
      (*playfield)[x][y + 1]->drawFigure (heroPm[actualPixmap], relx, rely - 16);

  // Merke alte Werte zum löschen der Figur
  mem_x = x;
  mem_y = y;
  mem_relx = relx;
  mem_rely = rely;
}

void KGrHero::dig(){
  if (direction == LEFT)
    digLeft();
  else
    if (direction == RIGHT)
      digRight();
}

void KGrHero::digLeft(){
  if (((*playfield)[x-1][y+1]->whatIam() == BRICK)&&
      (((*playfield)[x-1][y]->whatIam() == HLADDER)||
       ((*playfield)[x-1][y]->whatIam() == FREE)||
       ((*playfield)[x-1][y]->whatIam() == HOLE)))
    ((KGrBrick*)(*playfield)[x-1][y+1])->dig();
}

void KGrHero::digRight(){
  if (((*playfield)[x+1][y+1]->whatIam() == BRICK)&&
      (((*playfield)[x+1][y]->whatIam() == HLADDER)||
       ((*playfield)[x+1][y]->whatIam() == FREE)||
       ((*playfield)[x+1][y]->whatIam() == HOLE)))
    ((KGrBrick*)(*playfield)[x+1][y+1])->dig();
}

void KGrHero::setEnemyList(QList<KGrEnemy> *e){
  enemies = e;
}

bool KGrHero::standOnEnemy()
{
    int c = 0;
    int range = enemies->count();
    if (range > 0) {
	for (KGrEnemy * enemy = enemies->at (c); c < range;  ) {
	    enemy = enemies->at(c++);
	    // Test if hero's foot is at or just below enemy's head (tolerance
	    // of 4 pixels) and the two figures overlap in the X direction.
	    if ((((absy + 16) == enemy->gety()) ||
		 ((absy + 12) == enemy->gety())) &&
		(((absx - 16) <  enemy->getx()) &&
		 ((absx + 16) >  enemy->getx()))) {
		return true;
	    }
	}
    }
    return false;
}

void KGrHero::collectNugget(){

  if ((*playfield)[x][y]->whatIam() == NUGGET)
    {
      ((KGrFree *)(*playfield)[x][y])->setNugget(false);
      ((KGrFree *)(*playfield)[x][y])->redrawFigure (this);
      if (!(--nuggets))
	emit haveAllNuggets();	// sendet der Application dass alle Nuggets
			    // gesammelt sind, um versteckte Leitern zu zeigen
      emit gotNugget(250); // sendet der Application ein Nugget um Score zu erhöhen

    }
}

void KGrHero::loseNugget() {

    // Enemy trapped or dead and could not drop nugget (NO SCORE for this).
    if (! (--nuggets))
	emit haveAllNuggets();	// Sendet der Application dass alle Nuggets
			  // gesammelt sind, um versteckte Leitern zu zeigen.
}

bool KGrHero::isInEnemy(){

  int c=0;
  int range=enemies->count();
  if (range)
    for (KGrEnemy *enemy=enemies->at(c);c<range; )
      {enemy = enemies->at(c++);
      if (isInside(enemy->getx(),enemy->gety())||
	  isInside(enemy->getx()-15,enemy->gety())||
	  isInside(enemy->getx(),enemy->gety()-15))
	return true;}
  return false;
}

bool KGrHero::isInside(int enemyx, int enemyy){

 return ((absx >= enemyx)&&
	  (absx <= enemyx+15)&&
	  (absy >= enemyy)&&
	  (absy <= enemyy+15));
}


KGrHero :: ~KGrHero (){

  delete walkTimer;
  delete fallTimer;
}


/*******************************************************
                        _____
                       | ____|_ __   ___ _ __ ___  _   _
                       |  _| | '_ \ / _ \ '_ ` _ \| | | |
                       | |___| | | |  __/ | | | | | |_| |
                       |_____|_| |_|\___|_| |_| |_|\__, |
                                                   |___/

********************************************************/
KGrEnemy :: KGrEnemy (const QPixmap & pic1, const QPixmap & pic2, int x, int y)
  : KGrFigure (x, y)
{
  QPixmap pixmap;
  QImage image_blue, image_gold;

  actualPixmap = 18;

  walkCounter = 1;
  captiveCounter = 0;

  searchStatus = HORIZONTAL;

  birthX=x;
  birthY=y;

  pixmap = pic1;
  image_blue = pixmap.convertToImage ();
  pixmap = pic2;
  image_gold = pixmap.convertToImage ();
  for (int i = 0; i < 20; i++)
    {
      enemyPm[0][i].convertFromImage (image_blue.copy (i * 16, 0, 16, 16));
      enemyPm[1][i].convertFromImage (image_gold.copy (i * 16, 0, 16, 16));
    }
  captiveTimer = new QTimer (this);
  connect (captiveTimer,SIGNAL(timeout()),SLOT(captiveTimeDone()));
  connect (walkTimer, SIGNAL (timeout ()), SLOT (walkTimeDone ()));
  connect (fallTimer, SIGNAL (timeout ()), SLOT (fallTimeDone ()));

  walkFrozen = FALSE;
  fallFrozen = FALSE;
  captiveFrozen = FALSE;
}

int KGrEnemy::WALKDELAY = 0;
int KGrEnemy::FALLDELAY = 0;
int KGrEnemy::CAPTIVEDELAY = 0;

void KGrEnemy::doStep() {
    if (walkFrozen) {
	walkFrozen = FALSE;
	walkTimeDone();
    }
    if (fallFrozen) {
	fallFrozen = FALSE;
	fallTimeDone();
    }
    if (captiveFrozen) {
	captiveFrozen = FALSE;
	captiveTimeDone();
    }
}

void KGrEnemy::showState(char option)
{
  printf("(%02d,%02d) - Enemy  [%d]", x, y, enemyId);
  switch (option) {
      case 'p': printf ("\n"); break;
      case 's': printf (" nuggets %02d status %d walk-ctr %d ",
			nuggets, status, walkCounter);
	    printf ("dirn %d search %d capt-ctr %d\n",
			direction, searchStatus, captiveCounter);
	    printf ("                     rel (%02d,%02d) abs (%03d,%03d)",
			relx, rely, absx, absy);
	    printf (" pix %02d", actualPixmap);
	    printf (" mem %d %d %d %d", mem_x, mem_y, mem_relx, mem_rely);
	    if (walkFrozen) printf (" wBlock");
	    if (fallFrozen) printf (" fBlock");
	    if (captiveFrozen) printf (" cBlock");
	    printf ("\n");
	    break;
  }
}

void KGrEnemy::init(int a,int b)
{
  walkTimer->stop(); // alles stoppen bevor die Werte neu gesetzt
  fallTimer->stop(); // werden, da es sonst zu ungewollten Effekten
  captiveTimer->stop(); // kommen kann
  walkCounter = 1;
  captiveCounter = 0;

  x = mem_x = a;
  y = mem_y = b;
  relx = mem_relx = 0;
  rely = mem_rely = 0;

  absx=16*x;
  absy=16*y;

  actualPixmap = 19;

  status = STANDING;
}

void KGrEnemy::walkTimeDone ()
{
  if (KGrObj::frozen) {walkFrozen = TRUE; return; }

  // Check we are alive BEFORE checking for friends being in the way.
  // Maybe a friend overhead is blocking our escape from a brick.
  if ((*playfield)[x][y]->whatIam()==BRICK) {	// sollte er aber in einem Brick
    dieAndReappear();				// sein, dann stirbt er wohl
    return;			// Must leave "walkTimeDone" when an enemy dies.
    }

  if (! bumpingFriend()) {
    switch (direction) {
      case UP:		walkUp (WALKDELAY);
			if ((rely == 0) &&
			    ((*playfield)[x][y+1]->whatIam() == USEDHOLE))
			    // Enemy kletterte grad aus einem Loch hinaus
			    // -> gib es frei!
			    ((KGrBrick *)(*playfield)[x][y+1])->unUseHole();
			break;
      case DOWN:	walkDown (WALKDELAY, FALLDELAY); break;
      case RIGHT:	walkRight (WALKDELAY, FALLDELAY); break;
      case LEFT:	walkLeft (WALKDELAY, FALLDELAY); break;
      default:		// Switch search direction in KGoldrunner search (only).
			searchStatus = (searchStatus==VERTIKAL) ?
					HORIZONTAL : VERTIKAL;
			status = STANDING;
			break;
    }
    // wenn die Figur genau ein Feld gelaufen ist
    if (status == STANDING) { // dann suche den Helden
      direction = searchbestway(x,y,herox,heroy); // und
      if (walkCounter >= 4) {
        if (! nuggets)
	    collectNugget();
        else
	    dropNugget();
      }
      status = WALKING; // initialisiere die Zählervariablen und
      walkCounter = 1; // den Timer um den Held weiter
      walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE); // zu jagen
      startWalk ();
      }
  }
  else {
    // A friend is in the way.  Try a new direction, but not if leaving a hole.
    Direction dirn;
    dirn = searchbestway (x, y, herox, heroy);
    if ((dirn != direction) && ((*playfield)[x][y]->whatIam() != USEDHOLE)) {
      direction = dirn;
      status = WALKING;
      walkCounter = 1;
      relx = 0; absx = 16 * x;
      rely = 0; absy = 16 * y;
      startWalk ();
    }
    walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
  }
  showFigure();
}

void KGrEnemy::fallTimeDone ()
{
  if (KGrObj::frozen) {fallFrozen = TRUE; return; }

  if ((*playfield)[x][y+1]->whatIam() == HOLE) {  // wenn Enemy ins Loch fällt
    ((KGrBrick*)(*playfield)[x][y+1])->useHole(); // reserviere das Loch, damit
						  // kein anderer es benutzt und
    if (nuggets) {			  // er muss Gold vorher fallen lassen
      nuggets=0;
      switch ((*playfield)[x][y]->whatIam()) {
      case FREE:
      case HLADDER:	
        ((KGrFree *)(*playfield)[x][y])->setNugget(true); break;
      default:
	emit lostNugget(); break;		// Cannot drop the nugget here.
      }
    }
    emit trapped (75);				// Enemy trapped: score 75.
  }
  else if (walkCounter <= 1) {
	// Enemies collect nuggets when falling.
	if (!nuggets) {
	  collectNugget();
	}
  }

  if ((*playfield)[x][y]->whatIam()==BRICK) {	// sollte er aber in einem Brick
    dieAndReappear();				// sein, dann stirbt er wohl
    return;			// Must leave "fallTimeDone" when an enemy dies.
    }

  if (standOnEnemy()) {				// Don't fall into a friend.
    fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
    return;
  }

  if (walkCounter++ < 4){
    fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
    { rely+=STEP; absy+=STEP;}
  }
  else {
    rely = 0; y ++; absy=16*y;
    if ((*playfield)[x][y]->whatIam() == USEDHOLE) {
        captiveCounter = 0;
        status = CAPTIVE;
        captiveTimer->start((CAPTIVEDELAY * NSPEED) / speed, TRUE);
    }
    else if (!(canStand()||hangAtPole())) {
	fallTimer->start((FALLDELAY * NSPEED) / speed, TRUE);
	walkCounter=1;
    }
    else {
	status = STANDING;
	if (hangAtPole())
	  actualPixmap=(direction ==RIGHT)?8:12;
    }
  }
  if (status == STANDING) {
    status = WALKING;
    walkCounter = 1;
    direction = searchbestway(x,y,herox,heroy);
    walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
    startWalk ();
    if (!nuggets)
      collectNugget();
    else
      dropNugget();
  }
  showFigure();
}

void KGrEnemy::captiveTimeDone()
{
  if (KGrObj::frozen) {captiveFrozen = TRUE; return; }
  if ((*playfield)[x][y]->whatIam()==BRICK)
    dieAndReappear();
  else
    if (captiveCounter > 6){
      status = WALKING;
      walkCounter = 1;
      direction = UP;
      walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
      captiveCounter = 0;
    } else {
      captiveCounter ++;
      captiveTimer->start((CAPTIVEDELAY * NSPEED) / speed, TRUE);
      showFigure();
    }
}

void KGrEnemy::startSearching()
{
    // Called from "KGoldrunner::startPlaying" and "KGrEnemy::dieAndReappear".
    init(x,y);

    if (canStand()||((*playfield)[x][y+1]->whatIam()==USEDHOLE)) {
	status = WALKING;
	walkTimer->start ((WALKDELAY * NSPEED) / speed, TRUE);
    }
    else {
	status = FALLING;
	fallTimer->start ((FALLDELAY * NSPEED) / speed, TRUE);
    }

    walkCounter = 1;
    direction = searchbestway(x,y,herox,heroy);
    startWalk();
}

void KGrEnemy::collectNugget()
{
  if (((*playfield)[x][y]->whatIam() == NUGGET) && (nuggets == 0) &&
      (alwaysCollectNugget || ((int)(5.0*rand()/RAND_MAX+1.0) > 4))){
      ((KGrFree *)(*playfield)[x][y])->setNugget(false);
      ((KGrFree *)(*playfield)[x][y])->redrawFigure (this);
      nuggets=1;
  }
}

void KGrEnemy::dropNugget()
{
  if (((int)(DROPNUGGETDELAY*rand()/RAND_MAX+1.0) > DROPNUGGETDELAY-5) &&
      ((*playfield)[x][y]->whatIam() == FREE))    {
      ((KGrFree *)(*playfield)[x][y])->setNugget(true);
      ((KGrFree *)(*playfield)[x][y])->redrawFigure (this);
      nuggets=0;
  }
}

void KGrEnemy::showFigure ()
{
  eraseOldFigure ();
  (*playfield)[x][y]->drawFigure (enemyPm[nuggets][actualPixmap], relx, rely);
  if (relx != 0)
    if (relx < 0)
      (*playfield)[x-1][y]->drawFigure (enemyPm[nuggets][actualPixmap], relx + 16, rely);
    else
      (*playfield)[x+1][y]->drawFigure (enemyPm[nuggets][actualPixmap], relx - 16, rely);
  else if (rely != 0)
    if (rely < 0)
      (*playfield)[x][y-1]->drawFigure (enemyPm[nuggets][actualPixmap], relx, rely + 16);
    else
      (*playfield)[x][y+1]->drawFigure (enemyPm[nuggets][actualPixmap], relx, rely - 16);

  // Merke alte Werte zum löschen der Figur
  mem_x = x;
  mem_y = y;
  mem_relx = relx;
  mem_rely = rely;
}

bool KGrEnemy::canWalkUp()
{
  return (((*playfield)[x][y-1]->whatIam() != BRICK) &&
	  ((*playfield)[x][y-1]->whatIam() != BETON) &&
	  ((*playfield)[x][y-1]->whatIam() != FBRICK) &&
	  (((*playfield)[x][y]->whatIam() == USEDHOLE) ||
	   ((*playfield)[x][y]->whatIam() == LADDER)));
}

void KGrEnemy::startWalk ()
{
    switch (direction) {
    case UP:	break;
    case RIGHT:	if (hangAtPole())
		    actualPixmap = RIGHTCLIMB1;
		else
		    actualPixmap = RIGHTWALK1;
		break;
    case DOWN:	break;
    case LEFT:	if (hangAtPole())
		    actualPixmap = LEFTCLIMB1;
		else
		    actualPixmap = LEFTWALK1;
		break;
    default:	break;
    }
}

void KGrEnemy::dieAndReappear()
{
    bool looking;
    int i;

    eraseOldFigure();

    if (nuggets > 0) {
	nuggets = 0;			// Enemy died and could not drop nugget.
	emit lostNugget();		// NO SCORE for lost nugget.
    }
    emit killed (75);			// Killed an enemy: score 75.

    if (reappearAtTop) {
	// Follow Traditional rules: enemies reappear at top.
	looking = TRUE;
	y = 2;
	// Randomly look for a free spot in row 2.  Limit the number of tries.
	for (i = 1; ((i <= 3) && looking); i++) {
	    x = (int)((FIELDWIDTH * (float) rand()) / RAND_MAX) + 1;
	    switch ((*playfield)[x][2]->whatIam()) {
	    case FREE:
	    case HLADDER:
		looking = FALSE;
		break;
	    default:
		break;
	    }
	}
	// If unsuccessful, choose the first free spot in row 3 or below.
	while ((y<FIELDHEIGHT) && looking) {
	    y++;
	    x = 0;
	    while ((x<FIELDWIDTH) && looking) {
		x++;
		switch ((*playfield)[x][y]->whatIam()) {
		case FREE:
		case HLADDER:
		    looking = FALSE;
		    break;
		default:
		    break;
		}
	    }
	}
    }
    else {
	// Follow KGoldrunner rules: enemies reappear where they started.
	x = birthX;
	y = birthY;
    }

    // Enemy reappears and starts searching for the hero.
    startSearching();
}

Direction KGrEnemy::searchbestway(int ew,int eh,int hw,int hh)
{
  Direction dirn;

  if ((*playfield)[x][y]->whatIam() == USEDHOLE)	// Could not get out of
      return UP;					// hole (eg. brick above
							// closed): keep trying.

  if (!canStand() &&					// Cannot stand,
      !hangAtPole() &&					// not on pole, not
      !standOnEnemy() &&				// walking on friend and
      !((*playfield)[x][y+1]->whatIam() == HOLE))	// not just out of hole,
      return DOWN;					// so must fall.

  switch (searchStrategy) {

  // Traditional search strategy.
  case LOW:
  dirn = STAND;
  if (eh == hh) {					// Hero on same row.
    dirn = lowGetHero (ew, eh, hw);
  }
  if (dirn != STAND) return (dirn);			// Can go towards him.

  if (eh >= hh) {					// Hero above enemy.
    dirn = lowSearchUp (ew, eh, hh);		// Find a way up.
  }
  else {						// Hero below enemy.
    dirn = lowSearchDown (ew, eh, hh);		// Find way down to him.
    if (dirn == STAND) {
	dirn = lowSearchUp (ew, eh, hh);		// No go: try up.
    }
  }

  if (dirn == STAND) {					// When all else fails,
    dirn = lowSearchDown (ew, eh, eh - 1);		// find way below hero.
  }
  return dirn;
  break;

  // KGoldrunner search strategy.
  case MEDIUM:
  case HIGH:
  if(searchStatus==VERTIKAL){
    if (eh > hh)
      return searchupway(ew,eh);
    if (eh < hh)
      return searchdownway(ew,eh);
    return STAND;
  } else {
    if (ew > hw)
      return searchleftway(ew,eh);
    if (ew < hw)
      return searchrightway(ew,eh);
    return STAND;
  }
  break;
  }
  return STAND;
}

///////////////////////////////////////////////
// Methods for medium-level search strategy. //
///////////////////////////////////////////////

Direction KGrEnemy :: searchdownway(int ew,int eh){
int i,j;
 i=j=ew;
 if ( (*playfield)[ew][eh]->searchValue & CANWALKDOWN)
   return DOWN;
 else while((i>=0)||(j<=FIELDWIDTH)){
   if (i>=0)
     if ( (*playfield)[i][eh]->searchValue & CANWALKDOWN)
       return LEFT;
     else
       if (!(( (*playfield)[i--][eh]->searchValue & CANWALKLEFT) ||
	     (runThruHole && ( (*playfield)[i][eh]->whatIam() == HOLE))))
       i=-1;
   if (j<=FIELDWIDTH)
     if ( (*playfield)[j][eh]->searchValue & CANWALKDOWN)
       return RIGHT;
     else
       if (!(( (*playfield)[j++][eh]->searchValue & CANWALKRIGHT) ||
	     (runThruHole && ( (*playfield)[j][eh]->whatIam() == HOLE))))
       j=FIELDWIDTH+1;
 }
 return STAND;
}

Direction KGrEnemy :: searchupway(int ew,int eh){
int i,j;
 i=j=ew;
 if ( (*playfield)[ew][eh]->searchValue & CANWALKUP)
   return UP;
 else while((i>=0)||(j<=FIELDWIDTH)){// search for the first way up
   if (i>=0)
     if ( (*playfield)[i][eh]->searchValue & CANWALKUP)
       return LEFT;
     else
       if (!(( (*playfield)[i--][eh]->searchValue & CANWALKLEFT) ||
	     (runThruHole && ( (*playfield)[i][eh]->whatIam() == HOLE))))
       i=-1;
   if (j<=FIELDWIDTH)
     if ( (*playfield)[j][eh]->searchValue & CANWALKUP)
       return RIGHT;
     else
       if (!(( (*playfield)[j++][eh]->searchValue & CANWALKRIGHT) ||
	     (runThruHole && ( (*playfield)[j][eh]->whatIam() == HOLE))))
	 j=FIELDWIDTH+1;
 }
 // BUG FIX - Ian W., 30/4/01 - Don't leave an enemy standing in mid air.
 if (!canStand()) return DOWN; else return STAND;
}

Direction KGrEnemy :: searchleftway(int ew,int eh){
int i,j;
 i=j=eh;
 if ( ((*playfield)[ew][eh]->searchValue & CANWALKLEFT) || /* kann figur nach links laufen ?*/
	     (runThruHole && ( (*playfield)[ew-1][eh]->whatIam() == HOLE)))
   return LEFT;
 else while((i>=0)||(j<=FIELDHEIGHT)){ /* in den grenzen ?*/
   if (i>=0)
     if ( ((*playfield)[ew][i]->searchValue & CANWALKLEFT) || /* ein weg nach links- oben gefunden ?*/
	     (runThruHole && ( (*playfield)[ew-1][i]->whatIam() == HOLE)))
       return UP; /* geh nach oben */
     else
       if (!( (*playfield)[ew][i--]->searchValue & CANWALKUP)) /* sonst ...*/
	 i=-1;
   if (j<=FIELDHEIGHT)
     if ( ((*playfield)[ew][j]->searchValue & CANWALKLEFT) || /* ein weg nach links- unten gefunden ?*/
	     (runThruHole && ( (*playfield)[ew-1][j]->whatIam() == HOLE)))
       return DOWN; /* geh nach unten */
     else
       if (!( (*playfield)[ew][j++]->searchValue&CANWALKDOWN)) /* sonst ... */
       j=FIELDHEIGHT+1;
 }
 return STAND; /* default */
}

Direction KGrEnemy :: searchrightway(int ew,int eh)
{
  int i,j;
  i=j=eh;
  if ( ((*playfield)[ew][eh]->searchValue & CANWALKRIGHT) ||
	     (runThruHole && ( (*playfield)[ew+1][eh]->whatIam() == HOLE)))
    return RIGHT;
  else while((i>=0)||(j<=FIELDHEIGHT)){
    if (i>=0)
      if ( ((*playfield)[ew][i]->searchValue & CANWALKRIGHT) ||
	     (runThruHole && ( (*playfield)[ew+1][i]->whatIam() == HOLE)))
	return UP;
      else
	if (!( (*playfield)[ew][i--]->searchValue & CANWALKUP))
	i=-1;
    if (j<=FIELDHEIGHT)
      if ( ((*playfield)[ew][j]->searchValue & CANWALKRIGHT) ||
	     (runThruHole && ( (*playfield)[ew+1][j]->whatIam() == HOLE)))
	return DOWN;
      else
	if (!( (*playfield)[ew][j++]->searchValue & CANWALKDOWN))
	j=FIELDHEIGHT+1;
  }
  return STAND;
}

////////////////////////////////////////////
// Methods for low-level search strategy. //
////////////////////////////////////////////

Direction KGrEnemy::lowSearchUp (int ew, int eh, int hh)
{
    int i, ilen, ipos, j, jlen, jpos, deltah, rungs;

    deltah = eh - hh;			// Get distance up to hero's level.

    // Search for the best ladder right here or on the left.
    i = ew; ilen = 0; ipos = -1;
    while (i >= 1) {
	rungs = distanceUp (i, eh, deltah);
	if (rungs > ilen) {
	    ilen = rungs;		// This the best yet.
	    ipos = i;
	}
	if (searchOK (-1, i, eh))
	    i--;			// Look further to the left.
	else
	    i = -1;			// Cannot go any further to the left.
    }

    // Search for the best ladder on the right.
    j = ew; jlen = 0; jpos = -1;
    while (j < FIELDWIDTH) {
	if (searchOK (+1, j, eh)) {
	    j++;			// Look further to the right.
	    rungs = distanceUp (j, eh, deltah);
	    if (rungs > jlen) {
		jlen = rungs;		// This the best yet.
		jpos = j;
	    }
	}
	else
	    j = FIELDWIDTH+1;		// Cannot go any further to the right.
    }

    if ((ilen == 0) && (jlen == 0))	// No ladder found.
	return STAND;

    // Choose a ladder to go to.
    if (ilen != jlen) {			// If the ladders are not the same
					// length, choose the longer one.
	if (ilen > jlen) {
	    if (ipos == ew)		// If already on the best ladder, go up.
		return UP;
	    else
		return LEFT;
	}
	else
	    return RIGHT;
    }
    else {				// Both ladders are the same length.

	if (ipos == ew)			// If already on the best ladder, go up.
	    return UP;
	else if (ilen == deltah) {	// If both reach the hero's level,
	    if ((ew - ipos) <= (jpos - ew)) // choose the closest.
		return LEFT;
	    else
		return RIGHT;
	}
	else return LEFT;		// Else choose the left ladder.
    }
}

Direction KGrEnemy::lowSearchDown (int ew, int eh, int hh)
{
    int i, ilen, ipos, j, jlen, jpos, deltah, rungs, path;

    deltah = hh - eh;			// Get distance down to hero's level.

    // Search for the best way down, right here or on the left.
    ilen = 0; ipos = -1;
    i = (willNotFall (ew, eh)) ? ew : -1;
    rungs = distanceDown (ew, eh, deltah);
    if (rungs > 0) {
	ilen = rungs; ipos = ew;
    }

    while (i >= 1) {
	rungs = distanceDown (i - 1, eh, deltah);
	if (((rungs > 0) && (ilen == 0)) ||
	    ((deltah > 0) && (rungs > ilen)) ||
	    ((deltah <= 0) && (rungs < ilen) && (rungs != 0))) {
	    ilen = rungs;		// This the best way yet.
	    ipos = i - 1;
	}
	if (searchOK (-1, i, eh))
	    i--;			// Look further to the left.
	else
	    i = -1;			// Cannot go any further to the left.
    }

    // Search for the best way down, on the right.
    j = ew; jlen = 0; jpos = -1;
    while (j < FIELDWIDTH) {
	rungs = distanceDown (j + 1, eh, deltah);
	if (((rungs > 0) && (jlen == 0)) ||
	    ((deltah > 0) && (rungs > jlen)) ||
	    ((deltah <= 0) && (rungs < jlen) && (rungs != 0))) {
	    jlen = rungs;		// This the best way yet.
	    jpos = j + 1;
	}
	if (searchOK (+1, j, eh)) {
	    j++;			// Look further to the right.
	}
	else
	    j = FIELDWIDTH+1;		// Cannot go any further to the right.
    }

    if ((ilen == 0) && (jlen == 0))	// Found no way down.
	return STAND;

    // Choose a way down to follow.
    if (ilen == 0)
	path = jpos;
    else if (jlen == 0)
	path = ipos;
    else if (ilen != jlen) {		// If the ways down are not same length,
					// choose closest to hero's level.
	if (deltah > 0) {
	    if (jlen > ilen)
		path = jpos;
	    else
		path = ipos;
	}
	else {
	    if (jlen > ilen)
		path = ipos;
	    else
		path = jpos;
	}
    }
    else {				// Both ways down are the same length.
	if ((deltah > 0) &&		// If both reach the hero's level,
	    (ilen == deltah)) {		// choose the closest.
	    if ((ew - ipos) <= (jpos - ew))
		path = ipos;
	    else
		path = jpos;
	}
	else
	    path = ipos;		// Else, go left or down.
    }

    if (path == ew)
	return DOWN;
    else if (path < ew)
	return LEFT;
    else
	return RIGHT;
}

Direction KGrEnemy::lowGetHero (int ew, int eh, int hw)
{
    int i, inc, returnValue;

    inc = (ew > hw) ? -1 : +1;
    i = ew;
    while (i != hw) {
	returnValue = canWalkLR (inc, i, eh);
	if (returnValue > 0)
	    i = i + inc;		// Can run further towards the hero.
	else if (returnValue < 0)
	    break;			// Will run into a wall regardless.
	else
	    return STAND;		// Won't run over a hole.
    }

    if (i < ew)		return LEFT;
    else if (i > ew)	return RIGHT;
    else		return STAND;
}

int KGrEnemy::distanceUp (int x, int y, int deltah)
{
    int rungs = 0;

    // If there is a ladder at (x.y), return its length, else return zero.
    while ( (*playfield)[x][y - rungs]->whatIam() == LADDER) {
	rungs++;
	if (rungs >= deltah)		// To hero's level is enough.
	    break;
    }
    return rungs;
}

int KGrEnemy::distanceDown (int x, int y, int deltah)
{
    // When deltah > 0, we want an exit sideways at the hero's level or above.
    // When deltah <= 0, we can go down any distance (as a last resort).

    int rungs = -1;
    int exitRung = 0;
    bool canGoThru = TRUE;
    char objType;

    // If there is a way down at (x,y), return its length, else return zero.
    // Because rungs == -1, we first check that level y is not blocked here.
    while (canGoThru) {
	objType = (*playfield)[x][y + rungs + 1]->whatIam();
	switch (objType) {
	case BRICK:
	case BETON:
	case HOLE:			// Enemy cannot go DOWN through a hole.
	case USEDHOLE:
	    if ((deltah > 0) && (rungs <= deltah))
		exitRung = rungs;
	    if ((objType == HOLE) && (rungs < 0))
		rungs = 0;		// Enemy can go SIDEWAYS through a hole.
	    else
		canGoThru = FALSE;	// Cannot go through here.
	    break;
	case LADDER:
	case POLE:			// Can go through or stop.
	    rungs++;			// Add to path length.
	    if ((deltah > 0) && (rungs >= 0)) {
		// If at or above hero's level, check for an exit from ladder.
		if ((rungs - 1) <= deltah) {
		    // Maybe can stand on top of ladder and exit L or R.
		    if ((objType == LADDER) && (searchOK (-1, x, y+rungs-1) ||
						searchOK (+1, x, y+rungs-1)))
			exitRung = rungs - 1;
		    // Maybe can exit L or R from ladder body or pole.
		    if ((rungs <= deltah) && (searchOK (-1, x, y+rungs) ||
					      searchOK (+1, x, y+rungs)))
			exitRung = rungs;
		}
		else
		    canGoThru = FALSE;	// Should stop at hero's level.
	    }
	    break;
	default:
	    rungs++;			// Can go through.  Add to path length.
	    break;
	}
    }
    if (rungs == 1) {
	for (KGrEnemy *enemy=enemies->first();enemy!=0;enemy=enemies->next()) {
	    if((x*16==enemy->getx()) && (y*16+16==enemy->gety()))
		rungs = 0;		// Pit is blocked.  Find another way.
	}
    }
    if (rungs <= 0)
	return 0;			// There is no way down.
    else if (deltah > 0)
	return exitRung;		// We want to take an exit, if any.
    else
	return rungs;			// We can go down all the way.
}

bool KGrEnemy::searchOK (int direction, int x, int y)
{
    // Check whether it is OK to search left or right.
    if (canWalkLR (direction, x, y) > 0) {
	// Can go into that cell, but check for a fall.
	if (willNotFall (x+direction, y))
	    return TRUE;
    }
    return FALSE;			// Cannot go into and through that cell.
}

int KGrEnemy::canWalkLR (int direction, int x, int y)
{
    if (willNotFall (x, y)) {
	switch ((*playfield)[x+direction][y]->whatIam()) {
	case BETON:
	case BRICK:
	case USEDHOLE:
	    return -1; break;		// Will be halted in current cell.
	default:
	    // NB. FREE, LADDER, HLADDER, NUGGET and POLE are OK of course,
	    //     but enemies can also walk left/right through a HOLE and
	    //     THINK they can walk left/right through a FBRICK.

	    return +1; break;		// Can walk into next cell.
	}
    }
    else
	return 0;			// Will fall before getting there.
}

bool KGrEnemy::willNotFall (int x, int y)
{
    int c, cmax;
    KGrEnemy *enemy;

    // Check the ceiling.
    switch ( (*playfield)[x][y]->whatIam()) {
    case LADDER:
    case POLE:
	return TRUE; break;		// OK, can hang on ladder or pole.
    default:
	break;
    }

    // Check the floor.
    switch ( (*playfield)[x][y+1]->whatIam()) {

    // Cases where the enemy knows he will fall.
    case FREE:
    case HLADDER:
    case FBRICK:

    // N.B. The enemy THINKS he can run over a NUGGET, a buried POLE or a HOLE.
    // The last of these cases allows the hero to trap the enemy, of course.

    // Note that there are several Traditional levels that require an enemy to
    // be trapped permanently in a pit containing a nugget, as he runs towards
    // you.  It is also possible to use a buried POLE in the same way.

	cmax = enemies->count();
	for (c = 0; c < cmax; c++) {
	    enemy = enemies->at(c);
	    if ((enemy->getx()==16*x) && (enemy->gety()==16*(y+1)))
		return TRUE;		// Standing on a friend.
	}
	return FALSE; break;		// Will fall: there is no floor.

    default:
	return TRUE; break;		// OK, will not fall.
    }
}

void KGrEnemy::setEnemyList(QList<KGrEnemy> *e){
  enemies = e;
}

bool KGrEnemy::standOnEnemy()
{
    int c = 0;
    int range = enemies->count();
    if (range > 1) {
	for (KGrEnemy * enemy = enemies->at (c); c < range;  ) {
	    enemy = enemies->at(c++);
	    // Test if enemy's foot is at or just below enemy's head (tolerance
	    // of 4 pixels) and the two figures overlap in the X direction.
	    if ((((absy + 16) == enemy->gety()) ||
		 ((absy + 12) == enemy->gety())) &&
		(((absx - 16) <  enemy->getx()) &&
		 ((absx + 16) >  enemy->getx()))) {
		return true;
	    }
	}
    }
    return false;
}

bool KGrEnemy::bumpingFriend()
{
//  Enemies that are falling are treated as being invisible (i.e. a walking
//  enemy will walk through them or they will stop on that enemy's head).
//
//  If two enemies are moving in opposite directions, they are allowed to come
//  within two cell widths of each other (8 steps).  Then one must stop before
//  entering the next cell and let the other one go into it.  If both are about
//  to enter a new cell, the one on the right or above gives way to the one on
//  the left or below (implemented by letting the latter approach to 7 steps
//  apart before detecting an impending collision, by which time the first
//  enemy should have stopped and given way).
//
//  In all other cases, enemies are allowed to approach to 4 steps apart (i.e.
//  bumping a friend), before being forced to stop and wait.  If they somehow
//  get closer than 4 steps (i.e. overlapping), the lower ID enemy is allowed
//  to move out while the higher ID enemy waits.  This can happen if one enemy
//  falls into another or is reborn where another enemy is located.

    int c, cmax;
    KGrEnemy *enemy;
    int dx, dy;

    cmax = enemies->count();
    for (c = 0; c < cmax; c++) {
	enemy = enemies->at(c);
	if ((enemy->enemyId != this->enemyId) && (enemy->status != FALLING)) {
	    dx = enemy->getx() - absx;
	    dy = enemy->gety() - absy;
	    switch (direction) {
	    case LEFT:
		if ((dx >= -32) && (dx < 16) && (dy > -16) && (dy < 16)) {
		    if ((enemy->direction == RIGHT) &&
			(enemy->status == WALKING)  && (absx%16==0)) {
			return TRUE;
		    }
		    else if (dx >= -16) {
			if ((dx > -16) && (this->enemyId < enemy->enemyId))
			    return FALSE;
			else
			    return TRUE;	// Wait for the one in front.
		    }
		}
		break;
	    case RIGHT:
		if ((dx > -16) && (dx < 32) && (dy > -16) && (dy < 16)) {
		    if ((enemy->direction == LEFT) &&
			(enemy->status == WALKING) && (absx%16==0)) {
			return TRUE;
		    }
		    else if (dx <= 16) {
			if ((dx < 16) && (this->enemyId < enemy->enemyId))
			    return FALSE;
			else
			    return TRUE;	// Wait for the one in front.
		    }
		}
		break;
	    case UP:
		if ((dy >= -32) && (dy < 16) && (dx > -16) && (dx < 16)) {
		    if ((enemy->direction == DOWN) &&
			(enemy->status == WALKING) && (absy%16==0)) {
			return TRUE;
		    }
		    else if (dy >= -16) {
			if ((dy > -16) && (this->enemyId < enemy->enemyId))
			    return FALSE;
			else
			    return TRUE;	// Wait for the one above.
		    }
		}
		break;
	    case DOWN:
		if ((dy > -16) && (dy < 32) && (dx > -16) && (dx < 16)) {
		    if ((enemy->direction == UP) &&
			(enemy->status == WALKING) && (absy%16==0)) {
			return TRUE;
		    }
		    else if (dy <= 16) {
			if ((dy < 16) && (this->enemyId < enemy->enemyId))
			    return FALSE;
			else
			    return TRUE;	// Wait for the one below.
		    }
		}
		break;
	    default:
		break;
	    }
	}
    }
    return FALSE;
}

KGrEnemy :: ~KGrEnemy ()
{
  delete captiveTimer;
  delete walkTimer;
  delete fallTimer;
}
