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

#ifndef RMM_MESSAGE_H
#define RMM_MESSAGE_H

#include <RMM_Entity.h>
#include <RMM_Envelope.h>
#include <RMM_Body.h>
#include <RMM_BodyPart.h>
#include <RMM_Defines.h>
#include <RMM_Enum.h>

class RMessage : public RBodyPart {

	public:

		RMessage();
		RMessage(const RMessage &);
		RMessage(const QCString & s);

		virtual ~RMessage();

		RMessage & operator = (const RMessage & message);
		
		friend QDataStream & operator << (QDataStream & str, RMessage & m);

		QCString recipientListAsPlainString();

		void		addPart(RBodyPart * bp);
		void		removePart(RBodyPart * part);
		
		bool hasParentMessageID();

		void setStatus(RMM::MessageStatus status);
		RMM::MessageStatus status();

		const char * className() const { return "RMessage"; }

	protected:
		
		RMM::MessageStatus	status_;
};

#endif

