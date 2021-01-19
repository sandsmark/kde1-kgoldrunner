#include <kgoldrunner.h>
#include <kgoldrunner.moc>
#include <iostream.h>
#include <stdlib.h>

KGoldrunner::KGoldrunner(QWidget *parent, const char *name)
	: KTopLevelWidget(name)
{
	QPopupMenu *file_menu = new QPopupMenu();
	file_menu->insertItem(klocale->translate("&Neues Spiel"),ID_NEW);
	file_menu->insertSeparator();
	file_menu->insertItem(klocale->translate("&Schlie�en"),ID_EXIT);

	menuBar = new KMenuBar(this, "menubar");
	menuBar->insertItem(klocale->translate("&Datei"), file_menu);
	menuBar->insertSeparator();

	QString about = "KGoldrunner 0.1\n(C) ";
	about += (QString) klocale->translate("von") +
	  " Marco Kr�ger\ngrisu@cs.tu-berlin.de";
	menuBar->insertItem( klocale->translate("&Help"), 
			     KApplication::getKApplication()->getHelpMenu(TRUE, about ) );
	connect(file_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
	setMenu(menuBar);


	statusBar = new KStatusBar(this);
	initStatusBar();
	setStatusBar(statusBar);
	statusBar->show();

	loader = kapp->getIconLoader();

	enemies.setAutoDelete(TRUE);

	view = new KGoldrunnerWidget(this);
	view->resize(448,320);
	view->setFixedSize(448,320);
	view->setBackgroundColor(black);
	setView(view);

	for (int j=0;j<20;j++)
	  for (int i=0;i<28;i++) {
	    playfield[i+1][j+1]=new KGrFree(loader,false,view);
	    playfield[i+1][j+1]->move(i*16,j*16);
	  }
	for (int j=0;j<30;j++) {
	  playfield[j][0]=new KGrBeton();
	  playfield[j][21]=new KGrBeton();
	}
	for (int i=0;i<22;i++) {
	  playfield[0][i]=new KGrBeton();
	  playfield[29][i]=new KGrBeton();
	}
	
	hero = new KGrHero(loader,0,0);
	connect(hero,SIGNAL(gotNugget()),SLOT(incScore()));
	connect(hero,SIGNAL(haveAllNuggets()),SLOT(showHidden()));
	connect(hero,SIGNAL(caughtHero()),SLOT(herosDead()));
      	connect(hero,SIGNAL(leaveLevel()),SLOT(nextLevel()));
	hero->setPlayfield(&playfield);

    	enemy = NULL;
	newLevel=true;
	
	srand(1); // initialisiere Random-Generator
}

KGoldrunner::~KGoldrunner()
{
	delete toolBar;
	delete statusBar;
	delete menuBar;
}

void KGoldrunner::keyPressEvent(QKeyEvent *k) 
{ 
  if (!newLevel) { 
    if (!hero->started){ 
      enemyCount--;
      enemy=enemies.at(enemyCount);
	while (enemyCount>=0) {
	  enemy->startSearching(); 
	  enemyCount--;
	  if (enemyCount>=0)
	    enemy=enemies.at(enemyCount);
	}
      hero->start(); 
    }
    switch(k->key()){ 
    case Key_Up: hero->setKey(UP);break; 
    case Key_Right: hero->setKey(RIGHT);break; 
    case Key_Down: hero->setKey(DOWN);break; 
    case Key_Left: hero->setKey(LEFT);break; 
    case Key_Control: 
    case Key_Space: hero->dig();break; 
    case Key_K: herosDead();break;
    default:break; } 
  } 
}

void KGoldrunner::changeLevel(int l)
{
  QString tmp;
  tmp.setNum(l);
  tmp = tmp.rightJustify(3,'0');
  tmp.insert(0,klocale->translate("Level: "));
  statusBar->changeItem(tmp,ID_LEVEL);
}

void KGoldrunner::changeLifes(int l)
{
  QString tmp;
  tmp.setNum(l);
  tmp = tmp.rightJustify(3,'0');
  tmp.insert(0,klocale->translate("Leben: "));
  statusBar->changeItem(tmp,ID_LIFES);
}

void KGoldrunner::changeScore(int l)
{
  QString tmp;
  tmp.setNum(l);
  tmp = tmp.rightJustify(5,'0');
  tmp.insert(0,klocale->translate("Score: "));
  statusBar->changeItem(tmp,ID_SCORE);
}

void KGoldrunner::incScore()
{
  changeScore(++score);
}

void KGoldrunner::commandCallback(int i)
{
  switch (i)
    {
    case ID_NEW : newGame();break;
    case ID_EXIT : exit(0);break;
    default : break;
    }
}

void KGoldrunner::initStatusBar()
{
  statusBar->insertItem(klocale->translate("Leben: 005"),ID_LIFES); 
  statusBar->insertItem(klocale->translate("Score: 00000"),ID_SCORE);
  statusBar->insertItem(klocale->translate("Level: 000"),ID_LEVEL);
  statusBar->insertItem(klocale->translate("Systemlevel"),ID_LEVELPLACE);
  statusBar->insertItem(" KGoldrunner ",ID_DUMMY);
  statusBar->setAlignment(ID_DUMMY,KStatusBar::Right);
}



int KGoldrunner::loadLevel(int levelNo)
{
  int i,j;
  QString user,syst;

  user.setNum(levelNo); // INT -> QString
  user=user.rightJustify(3,'0'); // prepend zeros
  user.append(".grl"); // append goldrunnerlevel-sufix
  syst=user;
  user.prepend(KApplication::localkdedir()+"/share/apps/kgoldrunner/levels/level"); // prepend local path
  syst.prepend(KApplication::kde_datadir()+"/kgoldrunner/levels/level"); // prepend system path
       
  QFile ulevel(user);
  QFile slevel(syst);
  QFile openlevel;
    
  // gucken ob und welcher Level existiert

  if (!ulevel.exists())
    {
      if (!slevel.exists())
	{
	  KMsgBox::message(this,klocale->translate("lade Level"),
			   klocale->translate("Dieser Level ist nicht vorhanden.\nBitte fragen sie Ihren Systemadministrator"));
	  return 0;
	}
      else {
	  openlevel.setName(slevel.name());
	  statusBar->changeItem(klocale->translate("Systemlevel"),ID_LEVELPLACE);
	}
    } else
      {
	openlevel.setName(ulevel.name());
	statusBar->changeItem(klocale->translate("Userlevel"),ID_LEVELPLACE);
      }
  
  // �ffne Level zum lesen
  if (!openlevel.open(IO_ReadOnly))
    {
      KMsgBox::message(this,klocale->translate("lade Level"),
		       klocale->translate("Fehler beim �ffnen des Levels"));
      return 0;
    }
  
  nuggets = 0;
  enemyCount=0;
  
  // lade den Level
  for (j=1;j<21;j++)
    for (i=1;i<29;i++)
      {
	changeObject(openlevel.getch(),i,j);
      }
  if (newLevel)
    {
      hero->setEnemyList(&enemies);
      for (enemy=enemies.first();enemy != 0; enemy = enemies.next())
	enemy->setEnemyList(&enemies);
    }
  hero->setNuggets(nuggets);
  
  initSearchMatrix();
  return 1;
}


void KGoldrunner::newGame()
{
  enemyCount = 0;
  newLevel = true;
  changeLevel(1);
  level = 1;
  changeLifes(5);
  lifes = 5;
  changeScore(0);
  score = 0;
  enemies.clear();
  newLevel = true;;
  loadLevel(level);
  newLevel = false;
}

void KGoldrunner::changeObject(unsigned char kind, int i, int j)
{
  delete playfield[i][j];
  switch(kind) {
  case FREE: createObject(new KGrFree(loader,false,view),i,j);break;
  case LADDER: createObject(new KGrLadder(loader,view),i,j);break;
  case HLADDER: createObject(new KGrHladder(loader,view),i,j);break;
  case BRICK: createObject(new KGrBrick(loader,view),i,j);break;
  case BETON: createObject(new KGrBeton(loader,view),i,j);break;
  case FBRICK: createObject(new KGrFbrick(loader,view),i,j);break;
  case POLE: createObject(new KGrPole(loader,view),i,j);break;
  case NUGGET: createObject(new KGrFree(loader,true,view),i,j);nuggets++;break;
  case HERO: createObject(new KGrFree(loader,false,view),i,j);
    hero->init(i,j);
    hero->started = false;
    hero->showFigure();
    break;
  case ENEMY: createObject(new KGrFree(loader,false,view),i,j);
    if (newLevel){
      enemy = new KGrEnemy(loader,i,j);
      enemy->setPlayfield(&playfield);
      enemy->enemyId=enemyCount++;
      enemies.append(enemy);
    } else {
      enemy=enemies.at(enemyCount);
      enemy->enemyId=enemyCount++;
      enemy->setNuggets(0);
      enemy->init(i,j);
    }
    enemy->showFigure();
    break;
  default :  createObject(new KGrBrick(loader,view),i,j);break;
  } 
}

void KGoldrunner::paintEvent( QPaintEvent * )
{
  if (hero != NULL)
    hero->showFigure();
  for (enemy=enemies.first();enemy != 0; enemy = enemies.next())
    {
      enemy->showFigure();
    }
}

void KGoldrunner::createObject(KGrObj *o, int x, int y)
{
  playfield[x][y] = o;
  playfield[x][y]->move((x-1)*16,(y-1)*16);
  playfield[x][y]->show();
}

void KGoldrunner::deletelevel()
{
  int i,j;
  for (i=1; i<21;i++)
    for (j=1;j<29;j++)
      delete playfield[j][i];
}

void KGoldrunner::herosDead()
{
  if (--lifes>0)
    {
      enemyCount = 0;
      loadLevel(level);
      changeLifes(lifes);
    }
  else {
    enemyCount = 0;
    enemies.clear();
    newLevel=true;
    loadLevel(0);
    newLevel = false;
  }
}

void KGoldrunner::nextLevel()
{
  enemyCount = 0;
  enemies.clear();
  changeLevel(++level);
  newLevel=true;
  loadLevel(level);
  newLevel = false;
}

void KGoldrunner::showHidden()
{
  int i,j;
  for (i=1;i<21;i++)
    for (j=1;j<29;j++)
      if (playfield[j][i]->whatIam()==HLADDER)
	((KGrHladder *)playfield[j][i])->showLadder();
  initSearchMatrix();
}

void KGoldrunner::initSearchMatrix()
{
  int i,j;
  
  for (i=1;i<21;i++){
    for (j=1;j<29;j++)
      {
	if (playfield[j][i]->whatIam()==LADDER)
	  playfield[j][i]->searchValue=CANWALKLEFT+CANWALKRIGHT+CANWALKUP+CANWALKDOWN;
	else 
	  if ((playfield[j][i+1]->whatIam()==BRICK)||
	      (playfield[j][i+1]->whatIam()==HOLE)||
	      (playfield[j][i+1]->whatIam()==USEDHOLE)||
	      (playfield[j][i+1]->whatIam()==BETON))
	    playfield[j][i]->searchValue=CANWALKLEFT+CANWALKRIGHT;
	  else
	    if ((playfield[j][i]->whatIam()==POLE)||
		(playfield[j][i+1]->whatIam()==LADDER))
	      playfield[j][i]->searchValue=CANWALKLEFT+CANWALKRIGHT+CANWALKDOWN;
	    else
	      playfield[j][i]->searchValue=CANWALKDOWN;
	if(playfield[j][i-1]->blocker)
	  playfield[j][i]->searchValue &= ~CANWALKUP;
	if(playfield[j-1][i]->blocker)
	  playfield[j][i]->searchValue &= ~CANWALKLEFT;
	if(playfield[j+1][i]->blocker)
	  playfield[j][i]->searchValue &= ~CANWALKRIGHT;
	if(playfield[j][i+1]->blocker)
	  playfield[j][i]->searchValue &= ~CANWALKDOWN;
      }
  }
}

