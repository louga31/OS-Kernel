#include "panic.h"
#include "BasicRenderer.h"

void Panic(const char* message) {
	Renderer.Clear(Colors::BLUE);
	Renderer.SetCursorPosition(50, 50);
	Renderer.SetCursorLimits(50,50);

	Renderer.Println("----------KERNEL PANIC----------", Colors::WHITE);
	Renderer.Print("\n", Colors::WHITE);
	Renderer.Println("Quentin did a very bad job and the OS crashed", Colors::WHITE);
	Renderer.Println("Ping him on discord (louga31#0001) and dont forget to tell him how bad his code is", Colors::WHITE);
	Renderer.Print("\n", Colors::WHITE);
	Renderer.Print("Error message: ", Colors::WHITE);
	Renderer.Println(message, Colors::RED);
}
