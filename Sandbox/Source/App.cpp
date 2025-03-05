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
		PE_TRACE(TEXT("{0}"), event.GetString().GetString());
	}
};

class App : public Pawn::Application
{
public:
	App() 
	{
		PushLayer(new ExLayer());
	};
	~App() {};

};

Pawn::Application* Pawn::CreateApplication()
{
	return new App();
}