#pragma once

class CHack
{
public:
	CHack();
	~CHack();

	static void Render();
	static void Clear();
	static void Show(bool bShow);
	
public:
	static bool		m_bIsActive;
	static int			step;
};