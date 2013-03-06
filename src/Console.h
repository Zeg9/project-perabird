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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <vector>
#include <string>
#include <SDL/SDL.h>
#include "Gui.h"

class Console {
	public:
		Console();
		~Console();
		void print(std::string line);
		void event(SDL_Event &e);
		void render();
	private:
		std::vector<std::string> text;
		Gui::Font font;
};

#endif //__CONSOLE_H__
