/*
	Empath - Mailer for KDE
	
	Copyright (C) 1998 Rik Hemsley rik@kde.org
	
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

#ifndef EMPATHCOMPOSEWINDOW_H
#define EMPATHCOMPOSEWINDOW_H

// Qt includes
#include <qpopupmenu.h>

// KDE includes
#include <ktmainwindow.h>
#include <kapp.h>
#include <kstdaccel.h>

// Local includes
#include "EmpathDefines.h"
#include "EmpathEnum.h"
#include "EmpathURL.h"

class EmpathComposeWidget;
class EmpathSettingsDialog;
class RMessage;

/**
 * Holds a compose widget.
 */
class EmpathComposeWindow : public KTMainWindow
{
	Q_OBJECT

	public:
		
		EmpathComposeWindow(ComposeType t, const EmpathURL &);
		~EmpathComposeWindow();

	protected slots:

		// File menu slots
		void s_fileSendMessage();
		void s_fileSendLater();
		void s_fileSaveAs();
		void s_filePrint();
		void s_fileClose();
		
		// Edit menu slots
	
		void s_editUndo();
		void s_editRedo();
		void s_editCut();
		void s_editCopy();
		void s_editPaste();
		void s_editDelete();
		void s_editSelectAll();
		void s_editFindInMessage();
		void s_editFind();
		void s_editFindAgain();
		
		// Message menu slots
		void s_messageNew();
		void s_messageSaveAs();
		void s_messageCopyTo();

		// Help menu slots
		void s_help();
		void s_aboutEmpath();
		void s_aboutQt();

		void s_confirmDelivery	(bool);
		void s_confirmReading	(bool);
		void s_addSignature		(bool);
		void s_digitallySign	(bool);
		void s_encrypt			(bool);

	private:
	
		// General
		KApplication	* parentApp;
		KStdAccel		* accel;

		KMenuBar		* menu;
		KToolBar		* tool;
		KStatusBar		* status;
		
		KConfig			* config;
	
		QPopupMenu		* fileMenu_;
		QPopupMenu		* editMenu_;
		QPopupMenu		* messageMenu_;
		QPopupMenu		* helpMenu_;

		// Empath stuff

		EmpathComposeWidget		* composeWidget_;
		EmpathSettingsDialog	* settingsDialog_;
		
		// Setup methods
		void setupMenuBar();

		void setupToolBar();

		void setupStatusBar();
		
		int id_confirmDelivery_;
		int id_confirmReading_;
		int id_addSignature_;
		int id_digitallySign_;
		int id_encrypt_;
};

#endif
