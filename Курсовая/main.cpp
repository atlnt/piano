#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <vector>

using namespace std;
using namespace sf;

const int COOLDOWN = 1000;

string int_to_string(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}

class Key{
public:
	int x, y, w, h;
	bool pressed = false;
	bool isPlaying = false;
	Sound sound;
	Color color;
	RectangleShape body;
	Keyboard::Key key;
	Clock keyClock;
	Key(int _x, int _y, int _w, int _h, Color _color, SoundBuffer* buffer, Keyboard::Key _key){
		x = _x;
		y = _y;
		w = _w;
		h = _h;
		key = _key;
		color = _color;
		body = RectangleShape(Vector2f(w, h));
		body.setPosition(Vector2f(x, y));
		body.setFillColor(_color);
		body.setOutlineColor(Color::Black);
		body.setOutlineThickness(-2);

		sound.setBuffer(*buffer);
	}

	void draw(RenderWindow* window){
		if (pressed){
			body.setFillColor(Color(100, 100, 100, 255));
		}
		else{
			body.setFillColor(color);
		}
		window->draw(body);
	}

	bool contains(int _x, int _y){
		return _x > x && _x < (x + w) && _y > y && _y < (y + h);
	}

	void play(){
		if (isPlaying) return;
		sound.play();
		isPlaying = true;
		keyClock.restart();
	}

	void stop(){
		if (keyClock.getElapsedTime().asMilliseconds() < COOLDOWN) return;
		sound.stop();
		isPlaying = false;
	}

	bool keyPress(Keyboard::Key _key){
		if (pressed) return false;
		if (key == _key){
			pressed = true;
			play();
			return true;
		}
		return false;
	}

	bool keyRelease(Keyboard::Key _key){
		if (key == _key){
			isPlaying = false;
			pressed = false;
			stop();
			return true;
		}
		return false;
	}
};

int main()
{
	RenderWindow window(VideoMode(800, 400), "Piano");

	int soundID = 0;
	int keyID = 0;

	vector<Keyboard::Key> keyboardKeys;
	keyboardKeys.push_back(Keyboard::A);
	keyboardKeys.push_back(Keyboard::S);
	keyboardKeys.push_back(Keyboard::F);
	keyboardKeys.push_back(Keyboard::G);
	keyboardKeys.push_back(Keyboard::H);
	keyboardKeys.push_back(Keyboard::Z);
	keyboardKeys.push_back(Keyboard::X);
	keyboardKeys.push_back(Keyboard::C);
	keyboardKeys.push_back(Keyboard::V);
	keyboardKeys.push_back(Keyboard::B);
	keyboardKeys.push_back(Keyboard::N);
	keyboardKeys.push_back(Keyboard::M);
	keyboardKeys.push_back(Keyboard::Comma);

	vector<SoundBuffer> buffers;

	for (int i = 0; i < 13; ++i){
		SoundBuffer buffer;
		buffer.loadFromFile("sounds/pn" + int_to_string(9 + i) + ".wav");
		buffers.push_back(buffer);
	}

	vector<Key> keys;
	for (int i = 1; i < 7; ++i){
		if (i == 3) continue;
		keys.push_back(Key(100 * i - 25, 0, 50, 250, Color::Black, &buffers[soundID++], keyboardKeys[keyID++]));
	}
	for (int i = 0; i < 8; ++i){
		keys.push_back(Key(100 * i, 0, 100, 400, Color::White, &buffers[soundID++], keyboardKeys[keyID++]));
	}

	int lastPressed = 0;

	bool mousePressed = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left){
				mousePressed = true;
				int x = event.mouseButton.x;
				int y = event.mouseButton.y;
				for (int i = 0; i < keys.size(); ++i){
					if (keys[i].contains(x, y)){
						if (!keys[i].pressed){
							keys[i].isPlaying = false;
							keys[i].pressed = true;
							lastPressed = i;
							keys[i].play();
						}
						break;
					}
				}
			}

			if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left){
				keys[lastPressed].pressed = false;
				mousePressed = false;
			}

			if (event.type == Event::KeyPressed){
				for (int i = 0; i < keys.size(); ++i){
					if (keys[i].keyPress(event.key.code)) break;
				}
			}

			if (event.type == Event::KeyReleased){
				for (int i = 0; i < keys.size(); ++i){
					if (keys[i].keyRelease(event.key.code)) break;
				}
			}
		}

		for (int i = 0; i < keys.size(); ++i){
			if (!keys[i].pressed && keys[i].keyClock.getElapsedTime().asMilliseconds() > COOLDOWN){
				keys[i].stop();
			}
		}

		Vector2i pixelPos = Mouse::getPosition(window);
		Vector2f worldPos = window.mapPixelToCoords(pixelPos);
		int x = worldPos.x;
		int y = worldPos.y;

		if (mousePressed){
			for (int i = 0; i < keys.size(); ++i){
				if (keys[i].contains(x, y)){
					if (!keys[i].pressed){
						if (i != lastPressed){
							keys[lastPressed].pressed = false;
						}
						keys[i].isPlaying = false;
						keys[i].pressed = true;
						lastPressed = i;
						keys[i].play();
					}
					break;
				}
			}
		}

		window.clear();
		for (int i = keys.size() - 1; i >= 0; --i){
			keys[i].draw(&window);
		}
		window.display();
	}

	return 0;
}
