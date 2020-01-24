#pragma once
#include "Engine/Async/AbstractRequest.hpp"

#include <string>

typedef void (*LogFunction)(std::string text);

//-----------------------------------------------------------------

class RequestPrint : public AbstractRequest
{
public:
	void SetValue(int value);
	void SetOutput(LogFunction fnc);
	void Process() override;
	void Finish() override;

private:
	int m_value = 0;
	LogFunction m_outFnc = nullptr;
};

//-----------------------------------------------------------------
class Texture2D;
class Image;

class RequestToMakeImageFromTexture : public AbstractRequest
{
public:
	void Process() override;
	void Finish() override;
	void SetTexture( Texture2D* tex );
	void SetImage( Image* img );
	
private:
	Texture2D* m_texture = nullptr;
	Image* m_image = nullptr;
};

//-----------------------------------------------------------------

class RequestSaveImage : public AbstractRequest
{
public:
	void Process() override;
	void Finish() override;
	void SetImage( Image* img  );

private:
	Image* m_image = nullptr;

};