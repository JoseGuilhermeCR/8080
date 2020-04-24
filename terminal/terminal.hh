#ifndef __TERMINAL_HH__
#define __TERMINAL_HH__

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>
#include <memory>
#include <unordered_map>

class Terminal {
	private:
		sf::Vector2u _term_size;
		sf::RenderWindow _terminal;
		std::unique_ptr<sf::Font> _terminal_font;
		sf::Text _text;

		unsigned _char_per_row;
		unsigned _char_per_column;
		std::vector<char> _screen_buffer;
		unsigned _cursor;
		sf::RectangleShape _cursor_shape;

		bool _closed;
		bool _buffer_changed;
	public:
//		static std::unordered_map<sf::Keyboard::Key, char> terminal_charset_map;
		Terminal();
		~Terminal();

		void update();
		void write_to_screen(char);

		bool closed() const;
};

#endif
