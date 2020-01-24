#include "Engine/Async/Request.hpp"
#include "Engine/Async/Dispatcher.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Core/Image.hpp"

//-----------------------------------------------------------------

void RequestPrint::SetValue(const int value)
{
	this->m_value = value;
}


void RequestPrint::SetOutput(const LogFunction fnc)
{
	m_outFnc = fnc;
}


void RequestPrint::Process()
{
	m_outFnc("Starting processing request " + std::to_string(m_value) + "... \n");
}


void RequestPrint::Finish()
{
	m_outFnc("Finished request " + std::to_string(m_value) + "\n");
}

//-----------------------------------------------------------------

void RequestToMakeImageFromTexture::Process()
{
	m_texture->MakeImage(*m_image);
}

void RequestToMakeImageFromTexture::Finish()
{
	delete m_texture;
	m_texture = nullptr;

	RequestSaveImage* save_image_rq = new RequestSaveImage();
	save_image_rq->SetImage(m_image);
	Dispatcher::AddRequest(save_image_rq);
}

void RequestToMakeImageFromTexture::SetTexture(Texture2D* tex)
{
	m_texture = tex;
}

void RequestToMakeImageFromTexture::SetImage(Image* img)
{
	m_image = img;
}

//-----------------------------------------------------------------

void RequestSaveImage::Process()
{
	m_image->SaveImage(SCREENSHOT_DIR + "screenshot.png");
}

void RequestSaveImage::Finish()
{
	delete m_image;
	m_image = nullptr;
}

void RequestSaveImage::SetImage(Image* img)
{
	m_image = img;
}

//-----------------------------------------------------------------
