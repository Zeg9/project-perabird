/*
    Copyright (C) 2012 Zeg9
    
    This file is part of Project Perabird.
    Project Perabird is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Project Perabird is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Project Perabird.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Gui.h"
#include "Game.h"
#include "Resources.h"
#include "SimpleEventHandlers.h"
#include "Connection.h"
#include "Forms.h"

namespace Forms {
	struct ConnectEventHandler : Gui::EventHandler
	{
		ConnectEventHandler(Gui::Entry* _userEntry, Gui::PassEntry *_passEntry)
		 : userEntry(_userEntry), passEntry(_passEntry) {}
		virtual void operator()(Gui::Event &e)
		{
			if (e.type == Gui::MOUSEUP)
			{
				if (userEntry->getEntryText() == "" || passEntry->getEntryText() == "") return;
				GAME->connect("zeg9.strangled.net",5153,userEntry->getEntryText(),passEntry->getEntryText());
			}
		}
		private:
			Gui::Entry * userEntry;
			Gui::PassEntry * passEntry;
	};
	LoginForm::LoginForm() :
		label(RESOURCES->getGuiTheme("button"),"Login"),
		userEntry(RESOURCES->getGuiTheme("entry")),
		passEntry(RESOURCES->getGuiTheme("entry")),
		cancelButton(RESOURCES->getGuiTheme("button"),"Cancel"),
		acceptButton(RESOURCES->getGuiTheme("button"),"Accept")
	{
		using namespace Gui;
		label.setPosition(0,-16-2-32-4);
		label.setSize(128,32);
		label.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		add(&label);
		userEntry.setPosition(0,-16-2);
		userEntry.setSize(128,32);
		userEntry.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		userEntry.addEventHandler(new SimpleEventHandlers::PushOnReturn(&acceptButton));
		add(&userEntry);
		passEntry.setPosition(0,16+2);
		passEntry.setSize(128,32);
		passEntry.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		passEntry.addEventHandler(new SimpleEventHandlers::PushOnReturn(&acceptButton));
		add(&passEntry);
		cancelButton.setPosition(-32,16+2+32+4);
		cancelButton.setSize(64,32);
		cancelButton.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		cancelButton.addEventHandler(new SimpleEventHandlers::Exit());
		add(&cancelButton);
		acceptButton.setPosition(32,16+2+32+4);
		acceptButton.setSize(64,32);
		acceptButton.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		acceptButton.addEventHandler(new ConnectEventHandler(&userEntry,&passEntry));
		add(&acceptButton);
	}
	
	
	struct ChatSendEventHandler : Gui::EventHandler
	{
		ChatSendEventHandler(Gui::Entry*_entry) : entry(_entry) {}
		virtual void operator()(Gui::Event &e)
		{
			if (e.type == Gui::MOUSEUP)
			{
				GAME->sendMessage(entry->getEntryText());
				entry->setText("");
			}
		}
		private:
			Gui::Entry*entry;
	};
	
	ChatForm::ChatForm() :
		entry(RESOURCES->getGuiTheme("entry")),
		send(RESOURCES->getGuiTheme("button"),"Send")
	{
		using namespace Gui;
		entry.setPosition(0,-16-2);
		entry.setSize(256,32);
		entry.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		entry.addEventHandler(new SimpleEventHandlers::PushOnReturn(&send));
		add(&entry);
		send.setPosition(0,16+2);
		send.setSize(64,32);
		send.setAlignment(HALIGN_MIDDLE,VALIGN_MIDDLE);
		send.addEventHandler(new ChatSendEventHandler(&entry));
		add(&send);
	}
	
}
