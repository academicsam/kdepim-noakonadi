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

#include <qstring.h>
#include <qstrlist.h>
#include <qregexp.h>

#include <RMM_Token.h>
#include <RMM_Defines.h>
#include <RMM_Message.h>
#include <RMM_Envelope.h>
#include <RMM_Body.h>

RMessage::RMessage()
	:	REntity()
{
	rmmDebug("ctor");
}

RMessage::RMessage(const RMessage & m)
	:	REntity()
{
	rmmDebug("ctor");
}

RMessage::RMessage(const QCString & s)
	:	REntity(s)
{
	rmmDebug("ctor");
}

RMessage::~RMessage()
{
	rmmDebug("dtor");
}

	RMessage &
RMessage::operator = (const RMessage & m)
{
	rmmDebug("operator =");
	if (this == &m) return *this;
	envelope_ = m.envelope_;
	body_ = m.body_;
	REntity::operator = (m);
	return *this;
}

	int
RMessage::numberOfParts()
{
	parse();
	return body_.numberOfParts();
}

	void
RMessage::addPart(RBodyPart * bp)
{
	parse();
	body_.addPart(bp);
	_update();
}

	void
RMessage::removePart(RBodyPart * part)
{
	parse();
	body_.removePart(part);
	_update();
}

	void
RMessage::_update()
{
	type_ = (body_.numberOfParts() == 1) ? BasicMessage : MimeMessage;
}

	RMessage::MessageType
RMessage::type()
{
	parse();
	return type_;
}

	RBodyPart
RMessage::part(int index)
{
	parse();
	return body_.part(index);
}

	void
RMessage::parse()
{
	rmmDebug("parse() called");
	if (parsed_) return;
	rmmDebug("strRep_ = " + strRep_);

	int endOfHeaders = strRep_.find(QRegExp("\n\n"));
	
	if (endOfHeaders == -1) {
		rmmDebug("No end of headers ! - message is " +
			QString().setNum(strRep_.length()) + " bytes long");
		return;
	}
	
	envelope_.set(strRep_.left(endOfHeaders));

	// Now see if there's a Content-Type header in the envelope.
	// If there is, we might be looking at a multipart message.
	if (envelope_.has(RMM::HeaderContentType)) {
		
		// It has the header.
		RContentType contentType(envelope_.contentType());
		
		// If the header say multipart, we'll need to know the boundary.
		if (contentType.type() == "multipart") {
		
			RParameterListIterator it(contentType.parameterList());
			
			for (; it.current(); ++it) {
			
				if (it.current()->attribute() == "boundary") {
				
					// We've found the boundary.
					// The body needs the boundary, so it knows where to split.
					// It also needs the content type header, so it knows what
					// subtype the contentType has.
					// It also needs the content-transfer-encoding, so that it
					// can decode where necessary.
					body_.setBoundary(it.current()->value());
					body_.setContentType(contentType);
					body_.setCTE(envelope_.contentTransferEncoding());
					body_.setMultiPart(true);
					break;
				}
			}
		}
	}

	body_.set(strRep_.right(strRep_.length() - endOfHeaders));
	
	parsed_		= true;
	assembled_	= false;
}

	void
RMessage::assemble()
{
	parse();
	if (assembled_) return;
	rmmDebug("assemble() called");

	_update();

	strRep_ = envelope_.asString();

	if (type_ == BasicMessage) {
		
		strRep_ += body_.asString();
	
	} else {
		
		strRep_ += body_.asString();
	}
	assembled_ = true;
}

	void
RMessage::createDefault()
{
	envelope_.createDefault();
	body_.createDefault();
}

	REnvelope &
RMessage::envelope()
{
	parse();
	return envelope_;
}

	RBody &
RMessage::body()
{
	parse();
	return body_;
}

	Q_UINT32
RMessage::size()
{
	return strRep_.length();
}

	RMM::MessageStatus
RMessage::status()
{
	return status_;
}

	const char *
RMessage::className()
{
	return "RMessage";
}

