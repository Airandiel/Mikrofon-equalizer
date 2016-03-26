#include "stdafx.h"
#pragma comment(lib,"winmm.lib")

#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <mmsystem.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <queue>
#include <cmath>
#include <sstream>

using namespace std;

int main()
{
	srand(time(NULL));
	int width = 800;
	int high = 600;
	int highi = high - 20;
	int length = (width * 3) / 3;
	//double scale =0.2;
	double scale = 0.005;
	//double scale = 0.145;
	//int cuts = 00;
	int cuts = 350;
	int n = 100;
	vector<double> average;
	double ave=0;
	vector<double> aver;
	int indic = 0;
	int iter = 0;
	int endingIndic = 0;
	//const int frequency = 44100;
	const int frequency = 64;
	uint16_t maxi = 0;

	/*sf::ContextSettings settings;
	settings.antialiasingLevel = 8;*/
	sf::RenderWindow window(sf::VideoMode(width, high), "mikrofon", sf::Style::Default);
	sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Green);
	
	vector <int16_t> tab;
	//queue <uint16_t> qu;



	sf::Font font;
	//font.loadFromFile("IndieFlower.ttf");
	font.loadFromFile("arial.ttf");

	sf::Text oznaczenia_skali;
	oznaczenia_skali.setFont(font);
	oznaczenia_skali.setCharacterSize(10);
	oznaczenia_skali.setColor(sf::Color::Red);


	for (int i = 0; i < length; i++){
		tab.push_back(0);
		average.push_back(0);
		//qu.push(high / 2);
	}
	for (int i = 0; i < n; i++){
		aver.push_back(0);
	}

	while (window.isOpen())
	{
		int start = clock();
		/*for (int i = 0; i < (width / 2); i++){
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(i, (100) + (tab[i] / 2))),
				sf::Vertex(sf::Vector2f(i, (100) - (tab[i] / 2)))
			};
			line[0].color = sf::Color::Green;
			line[1].color = sf::Color::Green;

			window.draw(line, 1, sf::Lines);
			window.display();
			///here continue
		}*/
		// Fill the WAVEFORMATEX struct to indicate the format of our recorded audio
		//   For this example we'll use "CD quality", ie:  44100 Hz, stereo, 16-bit
		WAVEFORMATEX wfx = {};
		wfx.wFormatTag = WAVE_FORMAT_PCM;       // PCM is standard
		wfx.nChannels = 1;                      // 2 channels = stereo sound
		wfx.nSamplesPerSec = 44100;             // Samplerate.  44100 Hz
		wfx.wBitsPerSample = 16;                // 16 bit samples
		// These others are computations:
		wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
		wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;


		// Open our 'waveIn' recording device
		HWAVEIN wi;
		waveInOpen(&wi,            // fill our 'wi' handle
			WAVE_MAPPER,    // use default device (easiest)
			&wfx,           // tell it our format
			NULL, NULL,      // we don't need a callback for this example
			CALLBACK_NULL | WAVE_FORMAT_DIRECT   // tell it we do not need a callback
			);

		// At this point, we have our device, now we need to give it buffers (with headers) that it can
		//  put the recorded audio somewhere
		char buffers[2][frequency * 2 * 2 / 2];    // 2 buffers, each half of a second long
		WAVEHDR headers[2] = { {}, {} };           // initialize them to zeros
		for (int i = 0; i < 2; ++i)
		{
			headers[i].lpData = buffers[i];             // give it a pointer to our buffer
			headers[i].dwBufferLength = frequency * 2 * 2 / 2;      // tell it the size of that buffer in bytes
			// the other parts of the header we don't really care about for this example, and can be left at zero

			// Prepare each header
			waveInPrepareHeader(wi, &headers[i], sizeof(headers[i]));

			// And add it to the queue
			//  Once we start recording, queued buffers will get filled with audio data
			waveInAddBuffer(wi, &headers[i], sizeof(headers[i]));
		}

		// In this example, I'm just going to dump the audio data to a binary file
		std::ofstream outfile("my_recorded_audio.bin", std::ios_base::out | std::ios_base::binary);

		// Print some simple directions to the user
		std::cout << "Now recording audio.  Press Escape to stop and exit." << std::endl;

		// start recording!
		waveInStart(wi);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		
		
		
		//shape.setPosition(rand() % width, rand() % high);
		//window.draw(shape);
		//window.display();
		// Now that we are recording, keep polling our buffers to see if they have been filled.
		//   If they have been, dump their contents to the file and re-add them to the queue so they
		//   can get filled again, and again, and again
		while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))  // keep looping until the user hits escape
		{
			window.clear(sf::Color::Black);
			for (auto& h : headers)      // check each header
			{
				if (h.dwFlags & WHDR_DONE)           // is this header done?
				{
					
					// if yes, dump it to our file
					outfile.write(h.lpData, h.dwBufferLength);
					/*std::cout << h.reserved<<"\n";*/
					/*for (uint32_t i = 0; i < 10; i++)
					{
					std::cout << std::fixed << std::setprecision(8) << (int((uint8_t)*(h.lpData + i))) << " ";
					}*/
					
					for (uint32_t i = 0; i < frequency; i += 64)
					{
						
						int16_t temp = 0;
						//temp |= (((uint8_t)*(h.lpData + i + 3))) << 24;
						//temp |= (((uint8_t)*(h.lpData + i + 2))) << 16;
						temp |= (((uint8_t)*(h.lpData + i + 1))) << 8;
						temp |= (((uint8_t)*(h.lpData + i)));

						temp = abs(temp);
						if (temp > maxi){
							maxi = temp;
						}
						if ((temp)<65000){
							if (iter < n){
								aver.push_back(temp / n);

								aver.erase(aver.begin());
								ave += temp / n;

								average.push_back(ave);
								average.erase(average.begin());
								iter++;
							}
							else{
								aver.push_back(temp / n);

								ave += aver[aver.size()-1];
								ave -= aver[0];
								average.push_back(ave);

								aver.erase(aver.begin());
								average.erase(average.begin());
							}
							/*if (temp > average[average.size() - 1]){
								temp -= average[average.size() - 1];
							}
							else{
								temp = 0;
							}*/
								//temp = abs(temp);
							/*if (temp < (0.5 * average[average.size() - 1])){
								temp /= 50;
							}
							if (average[average.size() - 1] < 100){
								temp /= 100;
							}*/
								std::cout << temp << " ";
								/*if ((int)temp < 200){
									tab.push_back((int)temp/2);
								}
								if ((int)temp > 500){
									tab.push_back(500);
								}
								else{
									tab.push_back((int)temp);
								}*/
								tab.push_back(temp);
								tab.erase(tab.begin());
								//qu.push(temp);
								//qu.pop();
						}
						else{
							temp = 1;
							if (iter < n){
								aver.push_back(temp / n);

								aver.erase(aver.begin());
								ave += temp / n;

								average.push_back(ave);
								average.erase(average.begin());
								iter++;
							}
							else{
								aver.push_back(temp / n);

								ave += aver[aver.size() - 1];
								ave -= aver[0];
								average.push_back(ave);

								aver.erase(aver.begin());
								average.erase(average.begin());
							}
							//std::cout << tab[tab.size()-1] << " ";
							//tab.push_back(tab[tab.size()-1]);
							std::cout << "1" << " ";
							tab.push_back(1);
							tab.erase(tab.begin());
						}

						std::cout  << average[average.size() - 1] << endl;
						/*std::cout << average[average.size() - 1] << endl;
						tab.push_back(temp);
						tab.erase(tab.begin());*/
						
						//std::cout << std::fixed << std::setprecision(8) << (int((uint8_t)*(h.lpData + i))) << " ";
					}
					
					for (int i = 0; i < length; i++){
						///*
						//sf::RectangleShape line1(sf::Vector2f(1, (average[i])));
						//line1.setPosition(i, (high / 2) - (average[i])/ 2);*/

						//line1.setFillColor(sf::Color::Red);
						//window.draw(line1);

						/*sf::CircleShape circle;
						circle.setRadius(1);
						circle.setFillColor(sf::Color::Red);
						circle.setPosition(i, (high / 2) - (average[i]));

						sf::RectangleShape line1(sf::Vector2f(1, ((log(average[i]) / log(15))) / scale - cuts));
						line1.setPosition(i, (high / 2) - (log(average[i]) / log(15)) / (2 * scale) + cuts / 2);

						line1.setFillColor(sf::Color::Red);
						window.draw(line1);

						window.draw(circle);*/
	
						/*sf::RectangleShape line(sf::Vector2f(1, (((log((tab[i] )) / log(15))) / scale - cuts)));
						line.setPosition(i, ((high / 2) - (log((tab[i] )) / log(15)) / (2 * scale) + cuts / 2));*/
						//sf::RectangleShape line(sf::Vector2f(1, (((tab[i]) / (10)) / scale - cuts)));
						//line.setPosition(i, (high / 2) - abs(((tab[i]/10)) / (scale) + cuts / 2)/2);
						/*sf::RectangleShape line(sf::Vector2f(1, tab[i] / scale));
						line.setPosition(i, (high / 2)-(tab[i]/(2*scale)));*/
						sf::RectangleShape line(sf::Vector2f(1, tab[i]*highi/maxi));
						line.setPosition(i, (high / 2) - tab[i] * highi / (2*maxi));
						line.setFillColor(sf::Color::Green);
						window.draw(line);	

						
					}

					sf::RectangleShape scal(sf::Vector2f(1, highi));
					scal.setPosition(5, 0+(high-highi)*0.5);
					scal.setFillColor(sf::Color::Red);
					window.draw(scal);
					sf::RectangleShape scal1(sf::Vector2f(width, 1));
					scal1.setPosition(0, high/2);
					scal1.setFillColor(sf::Color::Red);
					window.draw(scal1);
					sf::RectangleShape line(sf::Vector2f(10, 1));
					line.setPosition(1, 3*(highi)/4);
					line.setFillColor(sf::Color::Red);
					window.draw(line);
					line.setPosition(1, 1 * high / 4);
					window.draw(line);
					line.setPosition(1, highi);
					window.draw(line);
					line.setPosition(1, high-highi);
					window.draw(line);

					int t = maxi / 2;
					stringstream ss;
					ss << (maxi/2);
					string punkts;
					ss >> punkts;
					/*string punkts;
					punkts = (string)punkty;*/
					oznaczenia_skali.setString(punkts);
					oznaczenia_skali.setPosition(12,-5 + 1 * highi / 4);
					window.draw(oznaczenia_skali);
					oznaczenia_skali.setPosition(12,-5 + 3 * highi / 4);
					window.draw(oznaczenia_skali);

					stringstream ss1;
					ss1 << maxi;
					ss1 >> punkts;
					/*string punkts;
					punkts = (string)punkty;*/
					oznaczenia_skali.setString(punkts);
					oznaczenia_skali.setPosition(12, -5 + highi);
					window.draw(oznaczenia_skali);
					oznaczenia_skali.setPosition(12, -5 + high-highi);
					window.draw(oznaczenia_skali);
					
					std::cout << "\n";
					// then re-add it to the queue
					h.dwFlags = 0;          // clear the 'done' flag
					h.dwBytesRecorded = 0;  // tell it no bytes have been recorded

					// re-add it  (I don't know why you need to prepare it again though...)
					waveInPrepareHeader(wi, &h, sizeof(h));
					waveInAddBuffer(wi, &h, sizeof(h));
				}
			}

			int stop = clock();
			while (stop - start < 50){
				stop = clock();
			}
			start = clock();
			window.display();
		}

		// Once the user hits escape, stop recording, and clean up
		window.display();
		waveInStop(wi);
		for (auto& h : headers)
		{
			waveInUnprepareHeader(wi, &h, sizeof(h));
		}
		waveInClose(wi);

	}

	// All done!
}
