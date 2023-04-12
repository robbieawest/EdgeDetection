#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdint.h>

struct col {
	int r = 0, g = 0, b = 0;
	col(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}

	void add(col o) {
		r += o.r;
		g += o.g;
		b += o.b;
	}

	void clip() {
		r = r < 0 || r > 255 ? (r > 255) * 255 : r;
		g = g < 0 || g > 255 ? (g > 255) * 255 : g;
		b = b < 0 || b > 255 ? (b > 255) * 255 : b;
	}

	sf::Color sfout() {
		return sf::Color(r, g, b);
	}

};


class Filter {
public:

	std::vector<std::vector<float>> kernel;

	Filter(std::vector<std::vector<float>> setter) {
		kernel = setter;
	}

	sf::Image convolute(sf::Image im) {
		sf::Image ret = im;

		for (int i = 0; i < im.getSize().y; i++) {
			for (int j = 0; j < im.getSize().x; j++) {
				
				//Looping through kernel bottom to top
				col sum(0, 0, 0);
				int ks = int(kernel.size()) / 2;

				for (int k = kernel.size() - 1 + -1 * ks; k >= -1 * ks; k--) {
					for (int l = kernel[0].size() - 1 + -1 * ks; l >= -1 * ks; l--) {

						sf::Vector2i target(j - l, i - k);
						
						if (target.x >= 0 && target.x < im.getSize().x && target.y >= 0 && target.y < im.getSize().y) {

							sf::Color c = im.getPixel(target.x, target.y);

							float kks = kernel[k + ks][l + ks];

							col add(c.r * kks, c.g * kks, c.b * kks);
							sum.add(add);
						}
					}
				}

				sum.clip();
				ret.setPixel(j, i, sum.sfout());
			}
		}

		return ret;
	}
};

sf::Image operator+(sf::Image im1, sf::Image im2) {
	sf::Image ret = im1;
	
	for (int i = 0; i < im1.getSize().y; i++) {
		for (int j = 0; j < im1.getSize().x; j++) {
			sf::Color p1 = im1.getPixel(j, i);
			sf::Color p2 = im2.getPixel(j, i);
			ret.setPixel(j, i, sf::Color(p1.r + p2.r, p1.g + p2.g, p1.b + p2.b));
		}
	}

	return ret;
}

int main() {

	std::cout << "Enter image path: " << std::endl;
	std::string path;
	getline(std::cin, path);

	std::cout << "Enter processing type: " << std::endl;
	std::cout << "1 >> Full Edge Detection\n2 >> Center Edge Detection\n3 >> Box Blur\n4 >> Gaussian Blur\n5 >> Sharpen" << std::endl;
	std::string type;
	getline(std::cin, type);

	sf::Image im;
	im.loadFromFile(path);

	sf::Vector2u size = im.getSize();
	sf::RenderWindow window(sf::VideoMode(size.x, size.y), path, sf::Style::Default);

	sf::Texture t;
	t.loadFromImage(im);
	sf::Sprite drawim;
	drawim.setTexture(t);


	Filter left({ {1, 0, -1},
				  {1, 0, -1},
				  {1, 0, -1}});

	Filter right({ {-1, 0, 1},
				   {-1, 0, 1},
				   {-1, 0, 1}});

	Filter top({    {1,   1,  1},
					{0,   0,  0},
					{-1, -1, -1}});

	Filter bottom({ {-1, -1, -1},
					{0,   0,  0},
					{1,   1,  1}});

	Filter center({ {-1, -1, -1},
					{-1,  8, -1},
					{-1, -1, -1}});

	float box9 = 1.0f / 9.0f;
	Filter box({ {box9, box9, box9},
				 {box9, box9, box9},
				 {box9, box9, box9}});

	Filter gaussian({ {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0},
					  {2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0},
					  {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0} });

	Filter sharpen({ {0,  -1,   0},
					 {-1,  5,  -1},
					 {0,  -1,   0}});






	while (window.isOpen()) {
		sf::Event evnt;
		while (window.pollEvent(evnt)) {
			switch (evnt.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::TextEntered:
					if (evnt.text.unicode == 's') {

						std::cout << "Starting convolution..." << std::endl;
						
						if (type == "1") {
							sf::Image im1 = left.convolute(im);
							sf::Image im2 = right.convolute(im);
							sf::Image im3 = top.convolute(im);
							sf::Image im4 = bottom.convolute(im);

							t.loadFromImage(im1 + im2 + im3 + im4);
						}
						else if (type == "2") {
							sf::Image im1 = center.convolute(im);
							t.loadFromImage(im1);
						}
						else if (type == "3") {
							sf::Image im1 = box.convolute(im);
							t.loadFromImage(im1);
						}
						else if (type == "4") {
							sf::Image im1 = gaussian.convolute(im);
							t.loadFromImage(im1);
						}
						else if (type == "5") {
							sf::Image im1 = sharpen.convolute(im);
							t.loadFromImage(im1);
						}
						else {
							std::cout << "Type input was invalid." << std::endl;
						}

						std::cout << "Finished." << std::endl;
					}
					break;
			}
		}

		window.clear(sf::Color(0, 0, 0));

		window.draw(drawim);

		window.display();
	}


	return 0;
}
