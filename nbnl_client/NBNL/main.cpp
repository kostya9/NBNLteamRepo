#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include "garage_communication.h"
#include "car_check.h"
#include "nbnl_db.h"
#include "html_gen.h"
#define SLEEP_TIME 5000
#define PORT 3

using namespace sf;

int main(void)
{
	int prevTime = 0.0f;
	int doorOpenedByCar = 0;
	int car_check_status = 0;
	start_car_check(6, &(car_check_status));
    garage_t * serial_garage = garage_new(3);
	db_con_t * con = db_connect("nbnl_garage.db");
    // Style::Fullscreen
    RenderWindow window(VideoMode (1366, 768), "Smart Garage API");

    Font font;
    font.loadFromFile("AnkePrint.ttf");
    Text text("", font, 50);
    text.setColor(Color(0,255,0));
    text.setStyle(Text::Bold);
    Text text2("", font, 50);
    text2.setColor(Color(0,255,0));
    text2.setStyle(Text::Bold);
    Text text3("", font, 50);
    text3.setColor(Color(0,0,255));
    text3.setStyle(Text::Bold);

	Text text9("", font, 50);
	
	text9.setColor(Color(153, 0, 0));
	text9.setStyle(Text::Bold);
	text9.setString("STATs");



    Image garage;
    garage.loadFromFile("garage.jpg");
    Texture garage_tex;
    garage_tex.loadFromImage(garage);
    Sprite garage_sprite;
    garage_sprite.setTexture(garage_tex);
    garage_sprite.setScale(2.74, 1.54);

    Image offButton;
    offButton.loadFromFile("offbutton.jpg");
    Texture offButton_t;
    offButton_t.loadFromImage(offButton);
    Sprite offButton_s;
    offButton_s.setTexture(offButton_t);
    offButton_s.setScale(0.05, 0.05);

    Image open;
    open.loadFromFile("open.png");
    open.createMaskFromColor(Color(255,255,255));
    Texture open_t;
    open_t.loadFromImage(open);
    Sprite open_s;
    open_s.setTexture(open_t);
    open_s.setScale(0.3, 0.3);
    open_s.setPosition(300, 200);

    Image close;
    close.loadFromFile("close.png");
    close.createMaskFromColor(Color(255,255,255));
    Texture close_t;
    close_t.loadFromImage(close);
    Sprite close_s;
    close_s.setTexture(close_t);
    close_s.setScale(0.36, 0.45);
    close_s.setPosition(310, 420);

	Image button;
	button.loadFromFile("button.png");
	button.createMaskFromColor(Color(255, 255, 255));
	Texture button_t;
	button_t.loadFromImage(button);
	Sprite button_s;
	button_s.setTexture(button_t);
	button_s.setPosition(900, 320);

    while(window.isOpen())
    {
		if (car_check_status == 0)
			puts("YEE");
		else
			puts("NOO");
        Event event;

        while(window.pollEvent(event))
        {
            if(event.type == Event::Closed)
                window.close();
        }

        if (event.type == Event::MouseMoved)
            {
                if(event.mouseMove.x > 319 && event.mouseMove.x < 520 && event.mouseMove.y > 255 && event.mouseMove.y < 373)
                {
                    open_s.setScale(0.34, 0.34);
                    open_s.setPosition(280, 180);
                }
                else if(event.mouseMove.x > 319 && event.mouseMove.x < 517 && event.mouseMove.y > 426 && event.mouseMove.y < 539)
                {
                    close_s.setScale(0.4, 0.5);
                    close_s.setPosition(290, 410);
                }
				else if (event.mouseMove.x > 885 && event.mouseMove.x < 1090 && event.mouseMove.y > 310 && event.mouseMove.y < 410)
				{
					button_s.setScale(1.1, 2.2);
					button_s.setPosition(890, 315);
					text9.setCharacterSize(55);
					text9.setPosition(910, 330);
				}
                else
                {
                    open_s.setScale(0.3, 0.3);
                    open_s.setPosition(300, 200);
                    close_s.setScale(0.36, 0.45);
                    close_s.setPosition(310, 420);
					button_s.setScale(1, 2);
					button_s.setPosition(900, 320);
					text9.setCharacterSize(50);
					text9.setPosition(915, 330);
                }

            }

		if (serial_garage != NULL)
		{
			/*if (event.type == Event::MouseButtonPressed)
			{
			if (event.mouseButton.button == Mouse::Left)
			{
			std::cout << "mouse x: " << event.mouseButton.x << std::endl;
			std::cout << "mouse y: " << event.mouseButton.y << std::endl;
			}
			}*/

			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if (event.mouseButton.x > 0 && event.mouseButton.x < 30 && event.mouseButton.y > 0 && event.mouseButton.y < 30)
					{
						std::cout << "Bye bye" << std::endl;
						window.close();
					}
				}
			}
			//statistics
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if(event.mouseButton.x > 885 && event.mouseButton.x < 1090 && event.mouseButton.y > 310 && event.mouseButton.y < 410){
						vector<garage_stats_entry> entries;
						db_get_garage_stats(con, entries);
						html_gen("out.html", entries);
						html_file_open("out.html");
					}
				}
			}

			//open
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if (event.mouseButton.x > 319 && event.mouseButton.x < 690 && event.mouseButton.y > 255 && event.mouseButton.y < 373)
					{
						int rc = garage_set_door_open(serial_garage);
						if (rc == 1)
							db_add_entry(con, "OPEN", time(NULL), "MANUALLY");
					}
				}
			}

			//close
			if (event.type == Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == Mouse::Left)
				{
					if (event.mouseButton.x > 319 && event.mouseButton.x < 517 && event.mouseButton.y > 426 && event.mouseButton.y < 539)
					{
						int rc = garage_set_door_close(serial_garage);
						if (rc == 1)
							db_add_entry(con, "CLOSE", time(NULL), "MANUALLY");
					}
				}
			}
			if (car_check_status == 0)
			{
				if (garage_get_door_status(serial_garage) == 0)
				{
					int rc = garage_set_door_open(serial_garage);
					if (rc == 1)
					{
						doorOpenedByCar = 1;
						db_add_entry(con, "OPEN", time(NULL), "AUTOMATICALLY");
					}
				}
			}
			if (car_check_status == 1 && doorOpenedByCar == 1)
			{
				int rc = garage_set_door_close(serial_garage);
				if (rc == 1)
				{
					doorOpenedByCar = 0;
					db_add_entry(con, "CLOSE", time(NULL), "AUTOMATICALLY");
				}
			}
			if (garage_get_door_status(serial_garage) == 1) {
				text.setString("GARAGE  IS  OPENED");
				text.setColor(Color(0, 255, 0));
			}
			else {
				text.setString("GARAGE  IS  CLOSED");
				text.setColor(Color(255, 0, 0));
			}

			if (garage_get_car_presence_status(serial_garage) == 1) {
				text2.setString("GARAGE  IS  FULL");
				text2.setColor(Color(0, 255, 0));
			}
			else {
				text2.setString("GARAGE  IS  EMPTY");
				text2.setColor(Color(255, 0, 0));
			}

			if (garage_get_person_motion_status(serial_garage) == 1) {
				text3.setString("LIGHT IS ON! (human)");
			}
			else if (garage_get_car_motion_status(serial_garage) == 1) {
				text3.setString("LIGHT IS ON! (car)");
			}
			else {
				text3.setString("LIGHT IS OFF!");
			}
		}
		else if (prevTime + 10 > time(NULL))
		{
			serial_garage = garage_new(PORT);
			prevTime = time(NULL);
		}



		text.setPosition(100, 100);
		text2.setPosition(700, 100);
        text3.setPosition(700, 500);
		
		window.clear(Color::White);
		window.draw(garage_sprite);
		if (serial_garage != NULL)
		{
			window.draw(offButton_s);
			window.draw(open_s);
			window.draw(close_s);
			window.draw(text2);
			window.draw(text3);
			window.draw(button_s);
			window.draw(text9);


		}
		else
		{
			text.setColor(Color(255, 0, 0));
			text.setString("Could not connect to the garage!");
		}
		

		window.draw(text);
        window.display();

    }
	if (serial_garage != NULL)
	{
		garage_delete(serial_garage);
	}
    return 0;
}

