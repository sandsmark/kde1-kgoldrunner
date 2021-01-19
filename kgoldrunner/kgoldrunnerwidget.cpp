/***************************************************************************
                          kgoldrunnerwidget.cpp  -  description
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

#include "kgoldrunnerwidget.h"
#include "kgoldrunnerwidget.moc"
#include "kgoldrunner.h"

KGoldrunnerWidget::KGoldrunnerWidget(KGoldrunner *parent, const char *name)
	: QWidget(parent, name)
{
    myParent = parent;
    setBackgroundMode (NoBackground);
    m = new QCursor ();
}

KGoldrunnerWidget::~KGoldrunnerWidget()
{
}

void KGoldrunnerWidget::mousePressEvent (QMouseEvent * m) {
    emit mouseClick (m->button ());
}

void KGoldrunnerWidget::mouseReleaseEvent (QMouseEvent * m) {
    emit mouseLetGo (m->button ());
}

QPoint KGoldrunnerWidget::getMousePos () {
    return (mapFromGlobal (m->pos()));
}

void KGoldrunnerWidget::paintEvent (QPaintEvent * ev)
{
  // Draw border around playfield (used as a buffer zone to catch mouse clicks).
  int		cw = 4*STEP;		// Cell width (= four steps).
  int		lw = cw/8;		// Line width (for edges of border).
  int		bw = 2*cw;		// Total border width (= 2 cells).
  int		mw = bw - 2*lw;		// Border main-part width.

  QPainter	p;
  QColor	c (192, 64, 64);	// Border's main colour.
  QColor	t (230, 230, 230);	// Text colour (off-white).
  QBrush	brush (c);		// Paints main part of border.
  QPen		textPen (t);		// Writes text in off-white colour.

  if (ev->rect().width() > 0)	// Use "ev": just to avoid compiler warnings.
	;

  p.begin (this);
  p.setBrush (brush);
  p.setPen (NoPen);

  QString s = p.fontInfo().family();
  int i = p.fontInfo().pointSize();
  p.setFont (QFont (s, i, QFont::Bold));

  // Draw main part of border, in the order: top, bottom, left, right.
  p.drawRect (lw, lw, FIELDWIDTH*cw + 2*bw - 2*lw, mw);
  p.drawRect (lw, FIELDHEIGHT*cw + bw + lw, FIELDWIDTH*cw + 2*bw - 2*lw, mw);
  p.drawRect (lw, bw - lw, mw, FIELDHEIGHT*cw + 2*lw);
  p.drawRect (FIELDWIDTH*cw + bw + lw, bw - lw, mw, FIELDHEIGHT*cw + 2*lw);

  // Draw outside edges of border, in the same order.
  p.setBrush (black);
  p.drawRect (0, 0, FIELDWIDTH*cw + 2*bw, lw);
  p.drawRect (0, FIELDHEIGHT*cw + 2*bw - lw, FIELDWIDTH*cw + 2*bw, lw);
  p.drawRect (0, lw, lw, FIELDHEIGHT*cw + 2*bw - 2*lw);
  p.drawRect (FIELDWIDTH*cw + 2*bw - lw, lw, lw, FIELDHEIGHT*cw + 2*bw - 2*lw);

  // Draw inside edges of border, in the same order.
  p.drawRect (bw - lw, bw - lw, FIELDWIDTH*cw + 2*lw, lw);
  p.drawRect (bw - lw, FIELDHEIGHT*cw + bw, FIELDWIDTH*cw + 2*lw, lw);
  p.drawRect (bw - lw, bw, lw, FIELDHEIGHT*cw);
  p.drawRect (FIELDWIDTH*cw + bw, bw, lw, FIELDHEIGHT*cw);

  p.setPen (textPen);
  p.drawText (0, 0, FIELDWIDTH*cw + 2*bw, bw, AlignCenter,
					       myParent->getTitle());
  p.end ();

  myParent->update();		// Make sure the hero and enemies appear.
}
