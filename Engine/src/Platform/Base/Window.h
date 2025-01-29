#pragma once
#include "pch.h"

// Base class for Window

class Window
{
public:
	virtual ~Window() {};

public:
	virtual uint32_t cr();
	
public:
	virtual void OnUpdate() = 0;

public:
	static Window* Create();

};

