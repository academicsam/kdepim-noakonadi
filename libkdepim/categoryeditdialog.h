/*
    This file is part of libkdepim.
    Copyright (c) 2000, 2001, 2002 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/
#ifndef KPIM_CATEGORYEDITDIALOG_H
#define KPIM_CATEGORYEDITDIALOG_H

#include <libkdepim/categoryeditdialog_base.h>

class KPimPrefs;

namespace KPIM {

class CategoryEditDialog : public CategoryEditDialog_base
{ 
    Q_OBJECT
  public:
    CategoryEditDialog( KPimPrefs *prefs, QWidget* parent = 0,
                        const char* name = 0, 
                        bool modal = FALSE, WFlags fl = 0 );
    ~CategoryEditDialog();

  public slots:
    void add();
    void remove();
    void modify();

    void slotOk();
    void slotApply();
    
  signals:
    void categoryConfigChanged();

  private slots:
    void editItem(QListViewItem *item);
    void slotTextChanged(const QString &text);

    KPimPrefs *mPrefs;
};

}

#endif
