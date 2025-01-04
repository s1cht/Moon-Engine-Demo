#include <PawnEngine.h>

class App : public Pawn::Application
{
public:
	App() {};
	~App() {};

};

Pawn::Application* Pawn::CreateApplication()
{
	return new App();
}