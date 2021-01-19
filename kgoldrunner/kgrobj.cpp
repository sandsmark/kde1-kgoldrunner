#include <kgrobj.h>
#include <kgrobj.moc>

KGrObj :: KGrObj (KIconLoader * loader, QWidget * parent, const char *name)
  :QWidget (parent, name)
{
  this->loader = loader;
  painter.begin (this);
  resize (16, 16);
  setFixedSize (16, 16);
  blocker = false;
}

void KGrObj::eraseFigure (int x, int y)
{
  erase (x, y, 16, 16);
}

void KGrObj::drawFigure (QPixmap f, int x, int y)
{
  painter.drawPixmap (x, y, f);
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

KGrObj :: ~KGrObj ()
{
  painter.end ();
}


KGrFree :: KGrFree (KIconLoader * loader, bool nug, QWidget * parent, const char *name)
  :KGrObj (loader, parent, name)
{
  freebg = loader->loadIcon ("hgbrick.xpm", 16, 16);
  nuggetbg = loader->loadIcon ("nugget.xpm", 16, 16);

  setNugget(nug);
}

void KGrFree :: setNugget(bool nug)
{
  if (!nug)
    {
      setBackgroundPixmap (freebg);
      iamA = FREE;
    }
  else
    {
      setBackgroundPixmap (nuggetbg);
      iamA = NUGGET;
    }
}

KGrFree :: ~KGrFree ()
{
}

KGrPole :: KGrPole (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrObj (loader, parent, name)
{
  setBackgroundPixmap (loader->loadIcon ("pole.xpm", 16, 16));
  iamA = POLE;
}

KGrPole :: ~KGrPole ()
{
}

KGrStones :: KGrStones (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrObj (loader, parent, name)
{
  blocker = true;
}

KGrStones :: ~KGrStones () 
{
}

/* +++++++++++++++ BRICK ++++++++++++++++ */

KGrBrick :: KGrBrick (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrStones (loader, parent, name)
{
  QPixmap pixmap;
  QImage image;
  pixmap = loader->loadIcon ("brick.xpm");
  image = pixmap.convertToImage ();
  for (int i = 0; i < 10; i++)
    {
      digpix[i].convertFromImage (image.copy (i * 16, 0, 16, 16));
    }
  setBackgroundPixmap (digpix[0]);
  // init timer
  timer = new QTimer (this);
  connect (timer, SIGNAL (timeout ()), SLOT (timeDone ()));
  iamA = BRICK;
}

void KGrBrick::dig (void)
{
  dig_counter = 1;
  iamA = HOLE;
  setBackgroundPixmap (digpix[1]);
  timer->start (DIGDELAY, TRUE);
}

void KGrBrick::timeDone ()
{
  if (dig_counter < 9) {
    dig_counter++;
    if (dig_counter == 5) // the hole is competely digged
      timer->start (DIGDELAY * HOLETIME, TRUE); // so we need a longer delay
    else {
      timer->start (DIGDELAY, TRUE);
      if (dig_counter >= 8)
	iamA = BRICK;
    }
  } else dig_counter = 0;
  setBackgroundPixmap (digpix[dig_counter]);
}

void KGrBrick :: useHole(){
  if (iamA == HOLE)
    iamA = USEDHOLE;
}

void KGrBrick :: unUseHole(){
  if (iamA == USEDHOLE)
    iamA = HOLE;
}

KGrBrick :: ~KGrBrick ()
{
  delete timer;
}

KGrFbrick :: KGrFbrick (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrStones (loader, parent, name)
{
  if (loader != 0)
    setBackgroundPixmap (loader->loadIcon ("fbrick.xpm", 16, 16));
  iamA = FBRICK;
}

KGrFbrick :: ~KGrFbrick ()
{
}

KGrBeton :: KGrBeton (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrStones (loader, parent, name)
{
  if (loader != 0)
    setBackgroundPixmap (loader->loadIcon ("beton.xpm", 16, 16));
  iamA = BETON;
}

KGrBeton :: ~KGrBeton ()
{
}

KGrLadder :: KGrLadder (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrObj (loader, parent, name)
{
  setBackgroundPixmap (loader->loadIcon ("ladder.xpm", 16, 16));
  iamA = LADDER;
}

KGrLadder :: ~KGrLadder ()
{
}

KGrHladder :: KGrHladder (KIconLoader * loader, QWidget * parent, const char *name)
  :KGrLadder (loader, parent, name)
{
  setBackgroundPixmap (loader->loadIcon ("hgbrick.xpm", 16, 16));
  ladderbg = loader->loadIcon("ladder.xpm",16,16);
  iamA = HLADDER;
}

void KGrHladder::showLadder()
{
  iamA = LADDER;
  setBackgroundPixmap(ladderbg);
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
KGrFigure :: KGrFigure (KIconLoader * loader, int x, int y)
{
  this->loader = loader;
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

int KGrFigure::herox =0;
int KGrFigure::heroy =0;

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
	  ((*playfield)[x][y+1]->whatIam() != FBRICK)&&
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

void KGrFigure::walkUp()
{
  actualPixmap = (actualPixmap == CLIMB1) ? CLIMB2 : CLIMB1;
  if (walkCounter++ < 4){ 
    if (canWalkUp()) { rely -= STEP; absy -= STEP;}
    walkTimer->start (WALKDELAY, TRUE);
  } else { 
    if (canWalkUp()) {
      y--; rely = 0; absy = 16*y;
    }
    status = STANDING;
  }
}

void KGrFigure::walkDown()
{
  if(hangAtPole())
    initFall(19);
  else {
    actualPixmap = (actualPixmap == CLIMB1) ? CLIMB2 : CLIMB1;
    if (walkCounter++ < 4)
      { if (canWalkDown()) {rely += STEP;absy += STEP;}
      walkTimer->start (WALKDELAY, TRUE);
      } else { if (canWalkDown()) {y++; rely = 0;absy = 16*y;}
      if (!canStand()) // kein Halt....urgs
	initFall(19);
      else status = STANDING;
      }
  }
}

void KGrFigure::walkLeft()
{
  if (walkCounter++)
    { if (++actualPixmap % 4)
      { if (canWalkLeft()) {relx -= STEP; absx -=STEP;}
      walkTimer->start (WALKDELAY, TRUE);
      } else { actualPixmap -= 4;
      if (canWalkLeft()) {x--; relx = 0; absx = x*16;}
      if (!(canStand()||hangAtPole())) // falling
	initFall(18);
      else  status = STANDING; }
    if (relx == -8) 
      if ((*playfield)[x-1][y]->whatIam() == POLE)
	actualPixmap = 14;
      else actualPixmap = 6;
    } else status = STANDING;
}

void KGrFigure::walkRight()
{
  if (walkCounter++) // wenn 0, soll sich Figur nur umdrehen
    { if (++actualPixmap % 4) // wenn true, dann ist kein vollst�ndiger Schritt gemacht
      { if (canWalkRight()) 
	{ relx += STEP;absx +=STEP; // nur vorw�rts gehen, wenn es auch m�glich ist 
	if (relx == 8) // Figur ist die H�lfte. Soll sie eine
	  if (hangAtPole())  // Pole anfassen oder loslassen ist jetzt
	    actualPixmap=10; //  der richtige Zeipunkt
	  else actualPixmap = 2;}
      walkTimer->start (WALKDELAY, TRUE); // neues Delay
      } else { actualPixmap -= 4; // Schritt war vollendet
      if (canWalkRight()) {x++;relx = 0;absx=16*x;} //gehe entg�ltig nach rechts
      if (!(canStand()||hangAtPole())) // kein Halt mehr...arrrgghhh
	initFall (19);
      else status = STANDING;} // Figur hat Halt  
    } else status = STANDING; // Figur sollte sich nur Umdrehen.
}

void KGrFigure::eraseOldFigure ()
{
  (*playfield)[mem_x][mem_y]->eraseFigure (mem_relx, mem_rely);
  
  if (mem_relx != 0)
    if (mem_relx < 0)
      (*playfield)[mem_x-1][y]->eraseFigure (mem_relx + 16, mem_rely);
    else
      (*playfield)[mem_x+1][y]->eraseFigure (mem_relx - 16, mem_rely);
  else if (mem_rely != 0)
    if (mem_rely < 0)
      (*playfield)[x][mem_y-1]->eraseFigure (mem_relx, mem_rely + 16);
    else
      (*playfield)[x][mem_y+1]->eraseFigure (mem_relx, mem_rely - 16);
}

void KGrFigure::initFall(int apm)
{
  status = FALLING;
  actualPixmap = apm;
  walkCounter=1;
  walkTimer->stop();
  fallTimer->start(FALLDELAY,TRUE);
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
KGrHero :: KGrHero (KIconLoader * loader, int x, int y)
  :KGrFigure (loader, x, y)
{
  QPixmap pixmap;
  QImage image;

  status = STANDING;
  actualPixmap = 18;
  WALKDELAY = 45;
  FALLDELAY = 50;

  pixmap = loader->loadIcon ("hero1.xpm");
  image = pixmap.convertToImage ();
  for (int i = 0; i < 20; i++)
    {
      heroPm[i].convertFromImage (image.copy (i * 16, 0, 16, 16));
    }

  herox=x;
  heroy=y;

  started = false;

  connect (walkTimer, SIGNAL (timeout ()), SLOT (walkTimeDone ()));
  connect (fallTimer, SIGNAL (timeout ()), SLOT (fallTimeDone ()));
}

/* Es ist Notwendig der eigentlichen Timerfunktion diese
   Startwalk vorzuschalten, um bei einem evtl. notwendigem
   Richtungswechsel der Figur diese Bewegung mit einzuf�gen */
void KGrHero::startWalk ()
{
  switch (nextDir)
    {
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
	  //	  fallTimer->start(FALLDELAY,TRUE);
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
  if (status != FALLING)//immer ausf�hren, ausser beim fallen
    { status = WALKING; // da sonst der FALLINGstatus wieder
    showFigure ();      // geaendert wird und der falsche Timer 
    //      walkTimer->start (WALKDELAY, TRUE);} // anspringt.
    }
}

void KGrHero::setKey(Direction key)
{
  nextDir = key;
}

void KGrHero::init(int a,int b)
{
  walkTimer->stop();
  fallTimer->stop();

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

void KGrHero::start(){

  started = true;
  if (!(canStand()||hangAtPole())) // Held muss wohl fallen...
    { status=FALLING;
    fallTimeDone();}
  else { status = STANDING;
  walkTimeDone();}

}

void KGrHero::walkTimeDone ()
{
  if ((status == STANDING) && (nextDir != STAND))
    startWalk();
  if (status != STANDING)
    { switch (direction)
      {
      case UP:if ((y==1)&&(nuggets<=0)) leaveLevel(); else walkUp();break;
      case DOWN:walkDown();break;
      case RIGHT:walkRight();break;
      case LEFT:walkLeft();break;
      default : 
	if (!canStand()||hangAtPole()) // falling
	  initFall(18);
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
      initFall(18);
    else
      walkTimer->start (WALKDELAY, TRUE);
  if(isInEnemy())
    caughtHero();
}

void KGrHero::fallTimeDone()
{
  if (!standOnEnemy())
    {
    if (walkCounter++ < 4)
      { // Held f�llt vier Positionen
	fallTimer->start(FALLDELAY,TRUE);
	{ rely+=STEP; absy+=STEP;}
      } else //Held ist ein Feld weitergefallen
	{ // Verschiebung der Figur zum 0-Punkt des Objekts (Brick etc...)
	  rely = 0; heroy = ++y; absy=16*y;// wird Null  und Figur eins runter gesetzt.
	  collectNugget(); // Zeit evtl. Nugget zu nehmen
	  if (!(canStand()||hangAtPole())) // Held muss wohl weiterfallen...
	    { fallTimer->start(FALLDELAY,TRUE);
	    walkCounter=1;
	    } else { // Held hat Boden (oder Feind) unter den F�ssen
	      status = STANDING; // oder h�ngt an Stange -> steh!
	      walkTimer->start(WALKDELAY,TRUE);
	      direction = (actualPixmap==19)?RIGHT:LEFT; 
	      if ((*playfield)[x][y]->whatIam() == POLE)
		actualPixmap = (direction==RIGHT)?8:12;
	      else 
		actualPixmap = (direction==RIGHT)?0:4;
	    }
	}
    showFigure();
    } else fallTimer->start(FALLDELAY,TRUE);
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

  // Merke alte Werte zum l�schen der Figur
  mem_x = x;
  mem_y = y;
  mem_relx = relx;
  mem_rely = rely;
}

void KGrHero::dig(){

  int t=1;
  if(status != STANDING)
    t=2;
  if (direction == LEFT) {
    if (((*playfield)[x-t][y+1]->whatIam() == BRICK)&&
	  (((*playfield)[x-t][y]->whatIam() == HLADDER)||
	   ((*playfield)[x-t][y]->whatIam() == FREE)||
	   ((*playfield)[x-t][y]->whatIam() == HOLE)))
	((KGrBrick*)(*playfield)[x-t][y+1])->dig();
    } else 
      if (direction == RIGHT)
	if (((*playfield)[x+t][y+1]->whatIam() == BRICK)&&
	    (((*playfield)[x+t][y]->whatIam() == HLADDER)||
	     ((*playfield)[x+t][y]->whatIam() == FREE)||
	    ((*playfield)[x+t][y]->whatIam() == HOLE)))
	  ((KGrBrick*)(*playfield)[x+t][y+1])->dig();
}

void KGrHero::setEnemyList(QList<KGrEnemy> *e){
  enemies = e;
}

bool KGrHero::standOnEnemy(){

  int c=0;
  int range=enemies->count()-1;
  if (range)
    for (  KGrEnemy *enemy=enemies->at(c);c<=range;){
      enemy = enemies->at(c++);
      if(((absx==enemy->getx()) &&((absy+16)==enemy->gety()))|| // um eine gewisse Tolleranz
	 ((absx==enemy->getx()) &&((absy+12)==enemy->gety()))) // zu haben diese Spanne von 4
	return true;
    }
  return false;
}

void KGrHero::collectNugget(){

  if ((*playfield)[x][y]->whatIam() == NUGGET)
    {
      ((KGrFree *)(*playfield)[x][y])->setNugget(false);
      if (!(--nuggets))
	haveAllNuggets(); // sendet der Application dass alle Nuggets gesammelt
	//  sind, um versteckte Leitern zu zeigen
      gotNugget(); // sendet der Application ein Nugget um Score zu erh�hen
      
    }
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
KGrEnemy :: KGrEnemy (KIconLoader * loader, int x, int y)
  : KGrFigure (loader, x, y)
{
  QPixmap pixmap;
  QImage image_blue, image_gold;

  actualPixmap = 18;
  WALKDELAY = 55;
  FALLDELAY = 100;
  CAPTIVEDELAY = 500;

  captiveCounter = 0;

  searchStatus = HORIZONTAL;

  birthX=x;
  birthY=y;

  pixmap = loader->loadIcon ("enemy1.xpm");
  image_blue = pixmap.convertToImage ();
  pixmap = loader->loadIcon ("enemy2.xpm");
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
}

void KGrEnemy::init(int a,int b)
{
  walkTimer->stop(); // alles stoppen bevor die Werte neu gesetzt
  fallTimer->stop(); // werden, da es sonst zu ungewollten Effekten 
  captiveTimer->stop(); // kommen kann

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
  if (!isInEnemy()){
    switch (direction)
      {
      case UP:  
	if ((*playfield)[x][y]->whatIam()==BRICK) {// sollte er aber in einem Brick sein, dann
	  x=birthX; y=birthY; relx=0;rely=0; startSearching();} // stirbt er wohl
	else { walkUp();
	  if ((rely==0)&&((*playfield)[x][y+1]->whatIam() == USEDHOLE)) // Enemy kletterte grad
	    ((KGrBrick *)(*playfield)[x][y+1])->unUseHole();// aus einem Loch hinaus-> gib es frei!
	} break;
      case DOWN:walkDown();break;
      case RIGHT:walkRight();break;
      case LEFT:walkLeft();break;
      default:searchStatus = (searchStatus==VERTIKAL)?HORIZONTAL:VERTIKAL;
	status = STANDING;
	break;
      }
    // wenn die Figur genau ein Feld gelaufen ist
    if (status == STANDING) // dann suche den Helden
      { direction = searchbestway(x,y,herox,heroy); // und
      status = WALKING; // initialisiere die Z�hlervariablen und
      walkCounter = 1; // den Timer um den Held weiter
      walkTimer->start (WALKDELAY, TRUE); // zu jagen
      if (!nuggets)
	collectNugget();
      else
	dropNugget();
      }     
  } else  walkTimer->start (WALKDELAY, TRUE);
  showFigure();
}

void KGrEnemy::fallTimeDone ()
{
  if ((*playfield)[x][y+1]->whatIam() == HOLE){ // wenn Enemy ins Loch f�llt
    ((KGrBrick*)(*playfield)[x][y+1])->useHole(); // reserviere das Loch, damit kein anderer es benutzt
    if( nuggets){  //und er muss Gold vorher fallen lassen
      ((KGrFree *)(*playfield)[x][y])->setNugget(true);
      nuggets=0;      
    } 
  }
  if ((*playfield)[x][y]->whatIam()==BRICK) {// sollte er aber in einem Brick sein, dann
    x=birthX; y=birthY; relx=0;rely=0; startSearching();} // stirbt er wohl
  
  if (walkCounter++ < 4){
    fallTimer->start(FALLDELAY,TRUE);
    { rely+=STEP; absy+=STEP;}
  } else { rely = 0; y ++; absy=16*y;
  if ((*playfield)[x][y]->whatIam() == USEDHOLE)
    {
      status = CAPTIVE;
      captiveTimer->start(CAPTIVEDELAY,TRUE);
    }else if (!(canStand()||hangAtPole()))
      {
	fallTimer->start(FALLDELAY,TRUE);
	walkCounter=1;
      } else {
	status = STANDING;
	if (hangAtPole())
	  actualPixmap=(direction ==RIGHT)?8:12;
      }
  }
  if (status == STANDING)
    {status = WALKING;
    walkCounter = 1;
    direction = searchbestway(x,y,herox,heroy);
    walkTimer->start (WALKDELAY, TRUE);
    if (!nuggets)
      collectNugget();
    else
      dropNugget();
    } 
  showFigure();
}

void KGrEnemy::captiveTimeDone()
{
  if ((*playfield)[x][y]->whatIam()==BRICK)
    { x=birthX; y=birthY; relx=0;rely=0;startSearching();}
  else 
    if (captiveCounter > 6){
      status = WALKING;
      walkCounter = 1;
      direction = UP;
      walkTimer->start (WALKDELAY, TRUE);
      captiveCounter = 0;
    } else {
      if(!isInEnemy())
	captiveCounter ++;
      captiveTimer->start(CAPTIVEDELAY,TRUE);
      showFigure();
    }
}

void KGrEnemy::startSearching()
{
  init(x,y);
  if (canStand()||((*playfield)[x][y+1]->whatIam()==USEDHOLE))
    {status = WALKING;
    walkTimer->start (WALKDELAY, TRUE);}
  else
    {status = FALLING;
    fallTimer->start (FALLDELAY, TRUE);}

  walkCounter = 1;
  direction = searchbestway(x,y,herox,heroy);
}

void KGrEnemy::collectNugget()
{
  if (((*playfield)[x][y]->whatIam() == NUGGET) &&
      ((int)(5.0*rand()/RAND_MAX+1.0) > 4) && (nuggets==0)){ 
    ((KGrFree *)(*playfield)[x][y])->setNugget(false);
    nuggets=1;
  }
}

void KGrEnemy::dropNugget()
{
  if (((int)(DROPNUGGETDELAY*rand()/RAND_MAX+1.0) > DROPNUGGETDELAY-5) &&
      ((*playfield)[x][y]->whatIam() == FREE))    { 
    ((KGrFree *)(*playfield)[x][y])->setNugget(true);
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

  // Merke alte Werte zum l�schen der Figur
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

Direction KGrEnemy :: searchbestway(int ew,int eh,int hw,int hh){
  if(searchStatus==VERTIKAL){
    if (eh > hh) 
      return searchupway(ew,eh,hw,hh);
    if (eh < hh)
      return searchdownway(ew,eh,hw,hh);
    return STAND;
  } else {
    if (ew > hw) 
      return searchleftway(ew,eh,hw,hh);
    if (ew < hw)
      return searchrightway(ew,eh,hw,hh);
    return STAND;
  }
}

Direction KGrEnemy :: searchdownway(int ew,int eh,int hw, int hh){
int i,j;
 i=j=ew; 
 if ( (*playfield)[ew][eh]->searchValue & CANWALKDOWN)
   return DOWN;
 else while((i>=0)||(j<=FIELDWIDTH)){
   if (i>=0)
     if ( (*playfield)[i][eh]->searchValue & CANWALKDOWN)
       return LEFT;
     else 
       if (!( (*playfield)[i--][eh]->searchValue & CANWALKLEFT))
       i=-1;
   if (j<=FIELDWIDTH)
     if ( (*playfield)[j][eh]->searchValue & CANWALKDOWN)
       return RIGHT;
     else 
       if (!( (*playfield)[j++][eh]->searchValue & CANWALKRIGHT))
       j=FIELDWIDTH+1;
 }
 return STAND;
}

Direction KGrEnemy :: searchupway(int ew,int eh,int hw, int hh){
int i,j;
 i=j=ew; 
 if ( (*playfield)[ew][eh]->searchValue & CANWALKUP)
   return UP;
 else while((i>=0)||(j<=FIELDWIDTH)){// search for the first way up
   if (i>=0)
     if ( (*playfield)[i][eh]->searchValue & CANWALKUP)
       return LEFT;
     else 
       if (!( (*playfield)[i--][eh]->searchValue & CANWALKLEFT))
       i=-1;
   if (j<=FIELDWIDTH)
     if ( (*playfield)[j][eh]->searchValue & CANWALKUP)
       return RIGHT;
     else 
       if (!( (*playfield)[j++][eh]->searchValue & CANWALKRIGHT))
	 j=FIELDWIDTH+1;
 }
 return STAND;
}

Direction KGrEnemy :: searchleftway(int ew,int eh,int hw, int hh){
int i,j;
 i=j=eh; 
 if ( (*playfield)[ew][eh]->searchValue & CANWALKLEFT) /* kann figur nach links laufen ?*/
   return LEFT;
 else while((i>=0)||(j<=FIELDHEIGHT)){ /* in den grenzen ?*/
   if (i>=0)
     if ( (*playfield)[ew][i]->searchValue & CANWALKLEFT) /* ein weg nach links- oben gefunden ?*/
       return UP; /* geh nach oben */
     else 
       if (!( (*playfield)[ew][i--]->searchValue & CANWALKUP)) /* sonst ...*/
	 i=-1;
   if (j<=FIELDHEIGHT) 
     if ( (*playfield)[ew][j]->searchValue & CANWALKLEFT) /* ein weg nach links- unten gefunden ?*/
       return DOWN; /* geh nach unten */
     else 
       if (!( (*playfield)[ew][j++]->searchValue&CANWALKDOWN)) /* sonst ... */
       j=FIELDHEIGHT+1;
 }
 return STAND; /* default */
} 

Direction KGrEnemy :: searchrightway(int ew,int eh,int hw, int hh)
{
  int i,j;
  i=j=eh; 
  if ( (*playfield)[ew][eh]->searchValue & CANWALKRIGHT)
    return RIGHT;
  else while((i>=0)||(j<=FIELDHEIGHT)){
    if (i>=0)
      if ( (*playfield)[ew][i]->searchValue & CANWALKRIGHT)
	return UP;
      else 
	if (!( (*playfield)[ew][i--]->searchValue & CANWALKUP))
	i=-1;
    if (j<=FIELDHEIGHT)
      if ( (*playfield)[ew][j]->searchValue & CANWALKRIGHT)
	return DOWN;
      else 
	if (!( (*playfield)[ew][j++]->searchValue & CANWALKDOWN))
	j=FIELDHEIGHT+1;
  }
  return STAND;
}

void KGrEnemy::setEnemyList(QList<KGrEnemy> *e){
  enemies = e;
}

bool KGrEnemy::standOnEnemy()
{
  for (KGrEnemy *enemy=enemies->first();enemy != 0; enemy = enemies->next())
    if((absx==enemy->getx()) &&(absy+16==enemy->gety()))
      return true;
  return false;
}

bool KGrEnemy::isInside(int enemyx, int enemyy)
{ 
  return ((absx >= enemyx) &&
	  (absx <= enemyx+15) &&
	  (absy >= enemyy) &&
	  (absy <= enemyy+15));
}

bool KGrEnemy::isInEnemy()
{
  int c=enemyId-1;
  if (c<0)
    return false;
  for (KGrEnemy *enemy=enemies->at(c);c>=0; c--){
    enemy = enemies->at(c);
    if (isInside(enemy->getx(),enemy->gety())||
	isInside(enemy->getx()-15,enemy->gety()))
      return true;}
  return false;
}

KGrEnemy :: ~KGrEnemy ()
{
  delete captiveTimer;
  delete walkTimer;
  delete fallTimer;
}
