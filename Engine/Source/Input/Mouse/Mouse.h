#pragma once

#include "Core/CoreTypes.h"

class Mouse
{
public:
	virtual ~Mouse() = 0;

public:
	bool IsLeftButtonUp()
	{
		return !m_LeftButtonDown;
	}

	bool IsLeftButtonDown()
	{
		return m_LeftButtonDown;
	}

	bool IsMiddleButtonUp()
	{
		return !m_MiddleButtonDown;
	}

	bool IsMiddleButtonDown()
	{
		return m_MiddleButtonDown;
	}

	bool IsRightButtonUp()
	{
		return !m_RightButtonDown;
	}

	bool IsRightButtonDown()
	{
		return m_RightButtonDown;
	}

	virtual bool IsSideButtonDown(uint8 sideButton)
	{
		return m_SideButtonDown[sideButton];
	};

protected:
	bool m_LeftButtonDown;
	bool m_MiddleButtonDown;
	bool m_RightButtonDown;

	bool m_SideButtonDown[16];

protected:
	friend class Input;
};

