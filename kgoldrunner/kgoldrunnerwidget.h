#ifndef KGOLDRUNNERWIDGET_H 
#define KGOLDRUNNERWIDGET_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <qwidget.h>

class KGoldrunnerWidget : public QWidget
{
	Q_OBJECT
public:
	KGoldrunnerWidget(QWidget *parent = 0, const char *name = 0);
	virtual ~KGoldrunnerWidget();
};

#endif // KGOLDRUNNERWIDGET_H 
