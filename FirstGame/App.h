#pragma once
#include "pch.h"
#include "Game.h"
#include <windows.h>

// Use some common namespaces to simplify the code
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

// the class definition for the core "framework" of our app
ref class App sealed : public IFrameworkView
{
public:
	//Initialise() is called behind the scenes when the App class is first created
	virtual void Initialize(CoreApplicationView^ AppView);
	virtual void SetWindow(CoreWindow^ Window);
	virtual void Load(String^ EntryPoint);
	virtual void Run();
	virtual void Uninitialize();

	// Application Lifecycle Event Handlers
	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args);
	void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
	void OnResuming(Platform::Object^ sender, Platform::Object^ args);

	//Key Press Event Handlers
	void KeyDown(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);
	void KeyUp(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);

	//Pointer Events
	void PointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void PointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);

	//Other Event Handlers
	void Closed(CoreWindow^ sender, CoreWindowEventArgs^ args);

private:

	bool WindowClosed;    // change to true to close the window
	CGame Game;

	std::array<bool, 4> wasd_keys = { false,false,false,false }; //array for storing whether keys w,a,s,d are pressed down
	bool spacePress = false;

	float pi = 3.14f;
	float startPointerX; //store last X coordinate of pointer before movement
	float startPointerY; // store last Y coordinate of pointer before movement
	float currPointerX;
	float currPointerY;
	float lastPointerX; //store last X coordinate of pointer before movement
	float lastPointerY; // store last Y coordinate of pointer before movement
	float lookAngleXZ = pi/2; // store look angle in XZ plane
	float lookAngleY = 0.0f; //store look angle in Y direction
	float speedDivX = 200.0f; //amount to divide change in X cursor coordinate by before updating lookAngleXZ
	float speedDivY = 200.0f; //amount to divide change in Y cursor coordinate by before updating lookAngleY

	DWORD moveStartTime;
	DWORD moveStopDelay = 50; //0.05sec

	bool mouseMoving = false;

};


// the class definition that creates an instance of our core framework class
ref class AppSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView()
	{
		return ref new App();    // create an App class and return it
	}
};

