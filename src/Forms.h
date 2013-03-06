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

#ifndef __FORMS_H__
#define __FORMS_H__

#include "Gui.h"

namespace Forms {
	class LoginForm : public Gui::Form {
		public:
			LoginForm();
		private:
			bool *done;
			Gui::Label label;
			Gui::Entry userEntry;
			Gui::PassEntry passEntry;
			Gui::Button cancelButton;
			Gui::Button acceptButton;
	};
	class ChatForm : public Gui::Form {
		public:
			ChatForm();
		private:
			Gui::Entry entry;
			Gui::Button send;
	};
}

#endif //__FORMS_H__
