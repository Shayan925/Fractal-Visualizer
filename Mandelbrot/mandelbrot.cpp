#include <SFML/Graphics.hpp>
#include <Windows.h>

using namespace sf;

// Screen resolution
const int WIDTH = GetSystemMetrics(SM_CXSCREEN);
const int HEIGHT = GetSystemMetrics(SM_CYSCREEN);

int max_iterations = 128;
int colour_choice = 1;
long double zoom = 1.0;
long double min_x = -2.5, max_x = 1;
long double min_y = -1, max_y = 1;


// Colour the gap between two points
// t is a value between 0 and 1
Color linear_interpolation(const Color& a, const Color& b, long double t) {
	return Color
	(
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
	);
}


//mapping function
long double MapValues(long double value, long double in_min, long double in_max, long double out_min, long double out_max) {
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int main(int argc, char* argv[]) {

	RenderWindow window(VideoMode(WIDTH, HEIGHT), "Mandelbrot Explorer");
	Texture texture;
	Sprite sprite;
	Image image, icon;
	Font font;
	Text text;
	Clock clock;
	Time time;

	image.create(WIDTH, HEIGHT);
	icon.loadFromFile("mandelbrot.png");
	font.loadFromFile("arial.ttf");
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(Color::White);
	window.setIcon(500, 500, icon.getPixelsPtr());

	while (window.isOpen()) {

		Event event;

		while (window.pollEvent(event)) {

			// Clicking on the 'X' icon or pressing the "END" key terminates the program
			if (event.type == Event::Closed || GetAsyncKeyState(VK_END)) 
				window.close();

			if (event.type == Event::KeyPressed) {
				// Movement speed
				long double h_delta = (max_x - min_x) * 0.05; 
				long double v_delta = (max_y - min_y) * 0.05;
				// Use WASD keys to move around
				if (event.key.code == Keyboard::W) { min_y -= v_delta, max_y -= v_delta; }
				if (event.key.code == Keyboard::A) { min_x -= h_delta, max_x -= h_delta; }
				if (event.key.code == Keyboard::S) { min_y += v_delta, max_y += v_delta; }
				if (event.key.code == Keyboard::D) { min_x += h_delta, max_x += h_delta; }

				// Pressing the numbers will set different colours for the mandelbrot
				switch (event.key.code) {
					case Keyboard::Num1:
						colour_choice = 1;
						break;
					case Keyboard::Num2:
						colour_choice = 2;
						break;
					case Keyboard::Num3:
						colour_choice = 3;
						break;
					case Keyboard::Num4:
						colour_choice = 4;
						break;
					case Keyboard::Num5:
						colour_choice = 5;
						break;
					case Keyboard::Num6:
						colour_choice = 6;
						break;
				}
			}

			// Use scroll wheel to zoom in on the position of the cursor
			if (event.type == Event::MouseWheelScrolled) {

				// Lambda function for zooming
				auto zoom_func = [&](long double factor) {

					long double temp1, temp2;

					// Get cursor world position
					long double cursor_x = MapValues(Mouse::getPosition(window).x, 0, WIDTH, min_x, max_x);
					long double cursor_y = MapValues(Mouse::getPosition(window).y, 0, HEIGHT, min_y, max_y);

					//set cursor as the center point and zoom
					temp1 = cursor_x - (max_x - min_x) / 2 / factor;
					max_x = cursor_x + (max_x - min_x) / 2 / factor;
					temp2 = cursor_y - (max_y - min_y) / 2 / factor;
					max_y = cursor_y + (max_y - min_y) / 2 / factor;

					min_x = temp1;
					min_y = temp2;
				};

				// Scrolling up will zoom in by a factor of 2
				if (event.mouseWheelScroll.delta > 0) {
					zoom_func(2);
					zoom *= 2;	
				} 
				// Scrolling down will zoom out by a factor of 2
				else {
					zoom_func(1.0 / 2); 
					zoom /= 2;	
				}
			}

			if (event.type == Event::MouseButtonPressed) {
				// Left click increases # of iterations which improves image quality
				if (event.mouseButton.button == Mouse::Left) 
					max_iterations *= 2;
				// Right click decreases # of iterations which worsens image quality
				if (event.mouseButton.button == Mouse::Right)
					max_iterations /= 2;
				// Set a limit for how low the image quality can be
				if (max_iterations < 1) 
					max_iterations = 1;
			}
		}

		window.clear();

// OpenMP creates another thread for the loop
#pragma omp parallel for

		for (int py = 0; py < HEIGHT; py++) {  
			for (int px = 0; px < WIDTH; px++) {  

				// Maps pixel coords to mandelbrot grid coords
				long double x0 = MapValues(px, 0, WIDTH, min_x, max_x);
				long double y0 = MapValues(py, 0, HEIGHT, min_y, max_y);
				long double x = 0.0, y = 0.0;
				int iterations;

				// Mathematics for producing the mandelbrot
				for (iterations = 0; iterations < max_iterations; iterations++) {
					long double xtemp = x * x - y * y + x0;
					y = 2 * x * y + y0;
					x = xtemp;

					if (x * x + y * y > 4) break;
				}

				// Vector storing colour values
				static std::vector<Color> colours;

				switch (colour_choice) {
				case 1:
					colours = {
						{0, 0, 0},
						{213, 67, 31},
						{251, 255, 121},
						{62, 223, 89},
						{43, 30, 218},
						{0, 255, 247}
					};
					break;
				case 2:
					colours = {
						{0, 7, 100},
						{32, 107, 203},
						{237, 255, 255},
						{255, 170, 0},
						{0, 2, 0},
					};
					break;
				case 3:
					colours = {
						{0, 0, 0},
						{35, 232, 156},
						{68, 255, 5},
						{250, 247, 87},
						{250, 185, 87},
						{255, 255, 255}
					};
					break;
				case 4:
					colours = {
						{0, 0, 0},
						{2, 52, 82},
						{6, 115, 122},
						{120, 11, 114},
						{120, 6, 36},
						{5, 0, 1}
					};
					break;
				case 5:
					colours = {
						{0, 0, 0},
						{164, 25, 224},
						{120, 30, 138},
						{250, 87, 96},
						{250, 87, 96},
						{255, 255, 255}
					};
					break;
				case 6:
					colours = {
						{0, 0, 0},
						{104, 61, 212},
						{90, 230, 227},
						{49, 212, 98},
						{246, 255, 0},
						{246, 0, 0}
					};
					break;
				}

				if (iterations == max_iterations) iterations = 0;
				long double decimal = long double(iterations) / max_iterations;
				static const auto last_index = colours.size() - 1;

				//scale variable to be in the range of colors and drop decimals
				decimal *= last_index;
				auto no_decimal = static_cast<size_t>(decimal);
				
				// Choose two colours to interpolate
				auto colour1 = colours[no_decimal];
				auto colour2 = colours[no_decimal+1];
				Color c = linear_interpolation(colour1, colour2, decimal - no_decimal);

				image.setPixel(px, py, Color(c));
			}
		}

		texture.loadFromImage(image);
		sprite.setTexture(texture);
		window.draw(sprite);
		
		time = clock.getElapsedTime();

		// Display the stats
		std::string str = " Max Iterations: " + std::to_string(max_iterations) + 
			"\n Zoom: " + std::to_string((long)zoom) + "x" +
			"\n FPS: " + std::to_string((long)std::round(1 / time.asSeconds()));

		text.setString(str);
		window.draw(text);
		window.display();

		clock.restart().asSeconds();
	}
	return 0;
}
