#include <PawnEngine.h>

class ExLayer : public Pawn::Layer
{
public:
	ExLayer() 
		: Layer(TEXT("Example"))
	{
	}

	void OnUpdate() override
	{
		//PE_INFO("ExLayer::Update");
	}

	void OnEvent(Pawn::Event& event)
	{
		//Pawn::String str = event.GetString();
		//::Pawn::Logger::GetClientLogger()->trace(L"{0}", str.GetString());
	}
};

class App : public Pawn::Application
{
public:
	App() 
	{
		//PushLayer(new ExLayer());
	};
	~App() {};

};

Pawn::Application* Pawn::CreateApplication()
{
	return new App();
}