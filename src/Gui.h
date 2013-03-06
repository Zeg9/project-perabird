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

#ifndef __GUI_H__
#define __GUI_H__

#include <map>
#include <string>
#include <vector>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

namespace Gui
{
	enum State {
		FIRST_STATE,
		INACTIVE,
		NORMAL,
		OVER,
		PRESS,
		LAST_STATE
	};
	std::string stateToString(State state);
	
	enum EventType {
		FIRST_EVENTTYPE,
		NONE,
		MOUSEOVER,
		MOUSEOUT,
		MOUSEDOWN,
		MOUSEUP,
		MOUSEDOWNOUT, // called when mouse button is pressed out of the element
		KEYDOWN,
		KEYUP,
		RESIZE, // called when window is resized - should be used in an EventHandler
		LAST_EVENTTYPE
	};
	std::string eventTypeToString(EventType eventType);
	struct Event {
		Event();
		EventType type;
		char key;
		int x,y;
		int button;
	};
	
	class Font {
		public:
			Font(std::string name);
			void render(char c);
			void render(std::string text);
		private:
			GLuint texture;
	};
	
	enum TextAlign {
		TEXTALIGN_LEFT,
		TEXTALIGN_CENTER,
		TEXTALIGN_RIGHT
	};
	enum HAlign {
		HALIGN_LEFT,
		HALIGN_RIGHT,
		HALIGN_MIDDLE
	};
	enum VAlign {
		VALIGN_TOP,
		VALIGN_BOTTOM,
		VALIGN_MIDDLE
	};
	
	class Theme {
		public:
			Theme(std::string path);
			GLuint getTexture(State state);
			Mix_Chunk* getSound(EventType eventType);
			Font *getFont();
		private:
			std::map<State,GLuint> textures;
			std::map<EventType,Mix_Chunk*> sounds;
			Font font;
	};
	
	struct EventHandler
	{
		virtual void operator()(Event &e) = 0;
	};
	
	class Element {
		public:
			Element(Theme*_theme, std::string _text, int _x, int _y, int _w, int _h);
			~Element();
			void addEventHandler(EventHandler* e);
			void render();
			void event(Event &e); // this calls elEvent, eventHandlers and plays event sounds eventually
			bool sdlEvent(SDL_Event &e); // this converts the given SDL_Event into a Gui::Event and send it to sdlEvent()
			void setPosition(int _x, int _y);
			void setSize(int _w, int _h);
			void setTextAlign(TextAlign _textAlign);
			void setAlignment(HAlign _hAlign, VAlign _vAlign);
			void calculatePosition();
			int getX();
			int getY();
			int getWidth();
			int getHeight();
			void setText(std::string _text);
			virtual std::string getText();
			virtual void setState(State _state);
			virtual State getState();
		protected:
			virtual void elEvent(Event &e) = 0; // element-specific event handling
			State state;
			Theme* theme;
			int xrel, yrel,x,y,w,h;
			HAlign hAlign; VAlign vAlign;
			std::string text;
			TextAlign textAlign;
			std::vector<EventHandler*> eventHandlers;
	};
	
	class Label : public Element {
		public:
			Label(Theme*_theme, std::string _text, int _x=0, int _y=0, int _w=64, int _h=32);
			virtual State getState(); // this will always return inactive, because a label doesn't need event handling
			// as well, label themes should only contain an _inactive image
		protected:
			virtual void elEvent(Event &e); // also, this is just here to make the class instanciable
	};
	
	class Button : public Element {
		public:
			Button(Theme*_theme, std::string _text, int _x=0, int _y=0, int _w=64, int _h=32);
		protected:
			virtual void elEvent(Event &e);
	};
	
	class Entry : public Element {
		public:
			Entry(Theme*_theme, std::string _text="", int _x=0, int _y=0, int _w=64, int _h=32);
			std::string getEntryText();
		protected:
			virtual std::string getText();
			virtual void elEvent(Event &e);
	};
	class PassEntry : public Entry {
		public:
			PassEntry(Theme*_theme, std::string _text="", int _x=0, int _y=0, int _w=64, int _h=32);
		private:
			std::string getText();
	};
	
	class Form {
		public:
			Form();
			~Form();
			void add(Element*e);
			void render(); // renders all elements
			bool sdlEvent(SDL_Event &e); // calls sdlEvent on all elements, also allows tab switching
			Element* getElement(int id);
			int getCount();
		private:
			std::vector<Element*> elements;
	};
}

#endif //__GUI_H__
