#pragma once
#include "pch.h"

// Base class for Window

class Window
{
public:
	virtual ~Window() {};

public:
	
public:
	virtual void OnUpdate() = 0;

public:
	static Window* Create();

};

