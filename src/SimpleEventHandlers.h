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

#ifndef __SIMPLEEVENTHANDLERS_H__
#define __SIMPLEEVENTHANDLERS_H__

#include "Gui.h"

namespace SimpleEventHandlers {
	struct Exit : Gui::EventHandler {
		virtual void operator()(Gui::Event &e);
	};
	struct PushOnReturn : Gui::EventHandler {
		PushOnReturn (Gui::Element*_toPush);
		virtual void operator()(Gui::Event &e);
		private:
			Gui::Element *toPush;
	};
}

#endif //__SIMPLEEVENTHANDLERS_H__
