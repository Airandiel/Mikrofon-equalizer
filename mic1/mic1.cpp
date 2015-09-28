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

using namespace std;

int main()
{
	srand(time(NULL));
	int width = 600;
	int high = 480;
	int length = (width * 2) / 3;
	double scale = 0.25;
	//const int frequency = 44100;
	const int frequency = 128;

	/*sf::ContextSettings settings;
	settings.antialiasingLevel = 8;*/
	sf::RenderWindow window(sf::VideoMode(width, high), "mikrofon", sf::Style::Default);
	sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Green);
	
	vector <uint16_t> tab;
	//queue <uint16_t> qu;
	for (int i = 0; i < length; i++){
		tab.push_back(5);
		//qu.push(high / 2);
	}

	while (window.isOpen())
	{
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
		wfx.nChannels = 2;                      // 2 channels = stereo sound
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
		std::ofstream outfile("my_recorded_audio.txt", std::ios_base::out | std::ios_base::binary);

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
						uint16_t temp = 0;
						temp |= (((uint8_t)*(h.lpData + i + 1))) << 8;
						temp |= (((uint8_t)*(h.lpData + i)));
						if ((int)temp < 65500){
							std::cout << temp << " ";
							tab.push_back(temp);
							tab.erase(tab.begin());
							//qu.push(temp);
							//qu.pop();
						}
						
						//std::cout << std::fixed << std::setprecision(8) << (int((uint8_t)*(h.lpData + i))) << " ";
					}
					
					for (int i = 0; i < length; i++){
						sf::RectangleShape line(sf::Vector2f(1, (log(tab[i]) / log(1.1)) / scale));
						line.setPosition(i, (high / 2)-(log(tab[i])/log(1.1))/(2*scale));
						/*sf::RectangleShape line(sf::Vector2f(1, tab[i] / scale));
						line.setPosition(i, (high / 2)-(tab[i]/(2*scale)));*/
						line.setFillColor(sf::Color::Green);
						window.draw(line);						
					}
					window.display();
					/*					for (int i = 0; i < (width / 2); i++){
											sf::Vertex line[] =
											{
												sf::Vertex(sf::Vector2f(i, (high / 2) + (tab[i] / 2))),
												sf::Vertex(sf::Vector2f(i, (high / 2) - (tab[i] / 2)))
											};

											window.draw(line, 1, sf::Lines);
											window.display();
											///here continue
										}*/
					/*shape.setPosition(rand() % width, rand() % high);
					window.draw(shape);*/
					
					
					std::cout << "\n";
					// then re-add it to the queue
					h.dwFlags = 0;          // clear the 'done' flag
					h.dwBytesRecorded = 0;  // tell it no bytes have been recorded

					// re-add it  (I don't know why you need to prepare it again though...)
					waveInPrepareHeader(wi, &h, sizeof(h));
					waveInAddBuffer(wi, &h, sizeof(h));
				}
			}
		}

		// Once the user hits escape, stop recording, and clean up

		waveInStop(wi);
		for (auto& h : headers)
		{
			waveInUnprepareHeader(wi, &h, sizeof(h));
		}
		waveInClose(wi);
	}

	// All done!
}
