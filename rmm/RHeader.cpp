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

#include <RMM_Enum.h>
#include <RMM_Header.h>
#include <RMM_Address.h>
#include <RMM_AddressList.h>
#include <RMM_DateTime.h>
#include <RMM_DispositionType.h>
#include <RMM_Mailbox.h>
#include <RMM_MailboxList.h>
#include <RMM_Mechanism.h>
#include <RMM_MessageID.h>
#include <RMM_Text.h>

RHeader::RHeader()
	:	RMessageComponent(),
		headerType_(RMM::HeaderUnknown),
		headerBody_(0)
{
	rmmDebug("ctor");
}

RHeader::RHeader(const RHeader & h)
	:	RMessageComponent()
{
	rmmDebug("copy ctor");
}

RHeader::~RHeader()
{
	rmmDebug("dtor");
	delete headerBody_;
	headerBody_ = 0;
}

	RHeader &
RHeader::operator = (const RHeader & h)
{
	rmmDebug("operator =");
	if (this == &h) return *this;

	headerName_ = h.headerName_;
	headerType_ = h.headerType_;

	if (headerBody_ != 0) {
		delete headerBody_;
		headerBody_ = 0;
	}
	
	headerBody_ = new RHeaderBody(*h.headerBody_);
	CHECK_PTR(headerBody_);
	
	RMessageComponent::operator = (h);
	assembled_	= false;
	return *this;
}

	QCString
RHeader::headerName()
{
	parse();
	return headerName_;
}

	RMM::HeaderType
RHeader::headerType()
{
	parse();
	return headerType_;
}

	RHeaderBody *
RHeader::headerBody()
{
	parse();
	return headerBody_;
}

	void
RHeader::setName(const QCString & name)
{
	headerName_ = name;
	assembled_ = false;
}

	void
RHeader::setType(RMM::HeaderType t)
{
	headerType_ = t;
	assembled_ = false;
}

	void
RHeader::setBody(RHeaderBody * b)
{
	headerBody_ = b;
	assembled_ = false;
}

	void
RHeader::parse()
{
	rmmDebug("parse() called");
	if (parsed_) return;
	rmmDebug("Need to parse");
	int split = strRep_.find(':');

	delete headerBody_;
	headerBody_ = 0;
	headerType_ = RMM::HeaderUnknown;

	if (split == -1) return;

	headerName_ = strRep_.left(split);
	headerName_ = headerName_.stripWhiteSpace();

	for (int i = 0; i <= 42; i++) {
		if (!stricmp((headerName_), RMM::headerNames[i])) {
			headerType_ = (RMM::HeaderType)i;
			break;
			rmmDebug("I'm of type " + QCString(RMM::headerNames[i]));
		}
	}

	if (headerType_ == RMM::HeaderUnknown) {
		rmmDebug("I'm an unknown header, \"" + headerName_ + "\"");
	}

	RHeaderBody * b;
	switch (RMM::headerTypesTable[headerType_]) {
		case RMM::Address:			b = new RAddress;			break;
		case RMM::AddressList:		b = new RAddressList;		break;
		case RMM::DateTime:			b = new RDateTime;			break;
		case RMM::DispositionType:	b = new RDispositionType;	break;
		case RMM::Mailbox:			b = new RMailbox;			break;
		case RMM::MailboxList:		b = new RMailboxList;		break;
		case RMM::Mechanism:		b = new RMechanism;			break;
		case RMM::MessageID:		b = new RMessageID;			break;
		case RMM::Text: default:	b = new RText;				break;
	}
	CHECK_PTR(b);

	QCString hb = strRep_.right(strRep_.length() - split - 1);
	hb = hb.stripWhiteSpace();
	b->set(hb);
	b->parse();
	headerBody_ = b;

	rmmDebug("strRep == " + strRep_);
	parsed_		= true;
	assembled_	= false;
}

	void
RHeader::assemble()
{
	rmmDebug("assemble() called");
	if (assembled_) return;

	if ((int)headerType_ > 42)
		headerType_ = RMM::HeaderUnknown;
	
	if (headerType_ != RMM::HeaderUnknown) {
		headerName_ = RMM::headerNames[headerType_];
	}

	strRep_ = headerName_;
	strRep_ += ':';
	strRep_ += ' ';

	if (headerBody_ != 0) {
		headerBody_->assemble();
		strRep_ += headerBody_->asString();
	}

	rmmDebug("assembled to: \"" + strRep_ + "\"");
	assembled_ = true;
}

	void
RHeader::createDefault()
{
}

