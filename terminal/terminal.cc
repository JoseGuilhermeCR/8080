#include "terminal.hh"
#include <string>
#include <SFML/Window/Event.hpp>
#include <iostream>

//std::unordered_map<sf::Keyboard::Key, char> Terminal::terminal_charset_map = {
//};

Terminal::Terminal()
	:
	_term_size({1280, 720}),
	_terminal({_term_size.x, _term_size.y}, "Terminal Emulator - by Jose Guilherme de C. Rodrigues", sf::Style::Close),
	_terminal_font(std::make_unique<sf::Font>()),
	_char_per_row(0),
	_char_per_column(0),
	_cursor(0),
	_closed(false)
{
	if (!_terminal_font->loadFromFile("Carlito-Regular.ttf")) {
		throw std::runtime_error("Couldn't load font.");
	}

	// Setup text.
	_text.setFont(*_terminal_font);
	_text.setCharacterSize(20);

	const unsigned c_size = _text.getCharacterSize();

	// Calculate number of characters that per row.
	_char_per_row = _term_size.x / c_size;
	_char_per_column = _term_size.y / c_size;

	_screen_buffer.reserve(_char_per_row * _char_per_column);
	for (unsigned i = 0; i < _char_per_column; ++i) {
		for (unsigned j = 0; j < _char_per_row; ++j) {
			_screen_buffer.push_back(0);
		}
	}

	// Setup cursor
	_cursor_shape.setFillColor(sf::Color::Black);
	_cursor_shape.setOutlineColor(sf::Color::White);
	_cursor_shape.setOutlineThickness(1.0f);
	_cursor_shape.setSize({static_cast<float>(c_size), static_cast<float>(c_size)});
	_cursor_shape.setPosition({static_cast<float>((_cursor % _char_per_row) * c_size), static_cast<float>((_cursor / _char_per_row) * c_size)});

	_screen_buffer[0] = 'H';
	_screen_buffer[1] = 'e';
	_screen_buffer[2] = 'l';
	_screen_buffer[3] = 'l';
	_screen_buffer[4] = 'o';
	_screen_buffer[5] = 'T';
	_screen_buffer[6] = 'h';
	_screen_buffer[7] = 'e';
	_screen_buffer[8] = 'r';
	_screen_buffer[9] = 'e';
}
Terminal::~Terminal() { }

void Terminal::update() {
	sf::Event event;
	while (_terminal.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			_closed = true;
		} else if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
				case sf::Keyboard::Right:
					if (_cursor < _screen_buffer.size() - 1)
						++_cursor;
					break;
				case sf::Keyboard::Left:
					if (_cursor > 0)
						--_cursor;
					break;
				case sf::Keyboard::Up:
					if (_cursor >= _char_per_row)
						_cursor -= _char_per_row;
					break;
				case sf::Keyboard::Down:
					if (_cursor + _char_per_row < _screen_buffer.size() - 1)
						_cursor += _char_per_row;
					break;
				case sf::Keyboard::A:
					if (_cursor < _screen_buffer.size() - 1) {
						_screen_buffer[_cursor++] = 'A';
					}
					break;
				case sf::Keyboard::B:
					if (_cursor < _screen_buffer.size() - 1) {
						_screen_buffer[_cursor++] = 'B';
					}
					break;
			}

			_cursor_shape.setPosition({static_cast<float>((_cursor % _char_per_row) * _text.getCharacterSize()), static_cast<float>((_cursor / _char_per_row) * _text.getCharacterSize())});
		}
	}

	_terminal.clear(sf::Color::Black);

	_terminal.draw(_cursor_shape);

	for (unsigned i = 0; i < _screen_buffer.size(); ++i) {
		_text.setString(_screen_buffer[i]);
		_text.setPosition({static_cast<float>((i % _char_per_row) * _text.getCharacterSize()), static_cast<float>((i / _char_per_row) * _text.getCharacterSize())});
		_terminal.draw(_text);
	}

	_terminal.display();
}

bool Terminal::closed() const {
	return _closed;
}

void Terminal::write_to_screen(char c) {
	_buffer_changed = true;
	_screen_buffer.push_back(c);
}

#ifdef TERMINAL_ONLY
int main() {
	Terminal terminal;

	while (!terminal.closed()) {
		terminal.update();
	}
}
#endif
