/*
    Empath - Mailer for KDE
    
    Copyright (C) 1998, 1999 Rik Hemsley rik@kde.org
    
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
*/

#ifdef __GNUG__
# pragma interface "EmpathConfigMaildirDialog.h"
#endif

#ifndef EMPATHCONFIGMAILDIRDIALOG_H
#define EMPATHCONFIGMAILDIRDIALOG_H

// Qt includes
#include <qdialog.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qspinbox.h>

// KDE includes
#include <kbuttonbox.h>

// Maildir includes
#include "EmpathDefines.h"

class EmpathMailboxMaildir;
class RikGroupBox;

/**
 * Configure a local mailbox.
 * Perhaps this should die and be replaced ?
 */
class EmpathConfigMaildirDialog : public QDialog
{
    Q_OBJECT

    public:
        
        EmpathConfigMaildirDialog(
                EmpathMailboxMaildir * mailbox,
                QWidget * parent = 0,
                const char * name = 0);

        ~EmpathConfigMaildirDialog() { empathDebug("dtor"); }

        void setMailbox(EmpathMailboxMaildir * mailbox);
        
    protected slots:

        void    s_OK();
        void    s_cancel();
        void    s_help();
        void    s_apply();
        void    s_default();
        void    s_browseMailboxPath();

    private:

        void saveData();
        void loadData();
        
        EmpathMailboxMaildir    * mailbox_;

        RikGroupBox        * rgb_server_;
        
        KButtonBox        * buttonBox_;
        
        QWidget            * w_server_;
        
        QGridLayout        * topLevelLayout_;
        QGridLayout        * serverGroupLayout_;
        
        QPushButton        * pb_OK_;
        QPushButton        * pb_cancel_;
        QPushButton        * pb_help_;
        QPushButton        * pb_apply_;
        QPushButton        * pb_default_;

        // w_server
        
        QLabel            * l_mailboxPath_;
        QLineEdit        * le_mailboxPath_;
        QPushButton        * pb_browseMailboxPath_;

        QSpinBox        * sb_mailCheckInterval_;

        QCheckBox        * cb_mailCheckInterval_;
        
        bool            applied_;
};

#endif
// vim:ts=4:sw=4:tw=78
