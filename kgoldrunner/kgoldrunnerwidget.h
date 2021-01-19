/***************************************************************************
                          kgoldrunnerwidget.h  -  description
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

#ifndef KGOLDRUNNERWIDGET_H
#define KGOLDRUNNERWIDGET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qpainter.h>

class KGoldrunner;		// Forward declare parent class.

class KGoldrunnerWidget : public QWidget
{
	Q_OBJECT
public:
	KGoldrunnerWidget(KGoldrunner *parent = 0, const char *name = 0);
	virtual ~KGoldrunnerWidget();
	QPoint getMousePos ();
signals:
	void mouseClick (int);
	void mouseLetGo (int);
protected:
	void mousePressEvent (QMouseEvent *);
	void mouseReleaseEvent (QMouseEvent *);
	void paintEvent( QPaintEvent * );
private:
	QCursor * m;
	KGoldrunner * myParent;
};

#endif // KGOLDRUNNERWIDGET_H
