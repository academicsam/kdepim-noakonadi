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

#ifndef RMM_GROUP_H
#define RMM_GROUP_H

#include <qstring.h>

#include <RMM_Mailbox.h>
#include <RMM_MailboxList.h>
#include <RMM_Address.h>
#include <RMM_Defines.h>

/**
 * An RGroup holds a name and a phrase, as per RFC822.
 */
class RGroup : public RAddress {

	public:

		RGroup();
		RGroup(const RGroup & group);
		RGroup(const QCString & s);
		RGroup & operator = (const RGroup & group);
		RGroup & operator = (const QCString & s);

		friend QDataStream & operator >> (
			QDataStream & s, RGroup & group);
		
		friend QDataStream & operator << (
			QDataStream & s, RGroup & group);

		virtual ~RGroup();

		void parse();
		void assemble();

		void createDefault();

		QCString name();

		QCString phrase();

		void setName(const QCString & name);

		void setPhrase(const QCString & phrase);

		RMailboxList & mailboxList();

		const char * className() const { return "RGroup"; }

	private:

		RMailboxList	mailboxList_;
		QCString		name_;
		QCString		phrase_;
};

#endif
