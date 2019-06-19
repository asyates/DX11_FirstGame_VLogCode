// Include the precompiled headers
#include "pch.h"
#include "App.h"


	//Initialise() is called behind the scenes when the App class is first created
void App::Initialize(CoreApplicationView^ AppView)
	{
		// Call OnActivated() when the Activated event is triggered. This will occur when application has finished loading.
		AppView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

		CoreApplication::Suspending +=
			ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

		CoreApplication::Resuming +=
			ref new EventHandler<Object^>(this, &App::OnResuming);

		WindowClosed = false;    // initialize to false
	}

//Place event handlers in here
void App::SetWindow(CoreWindow^ Window) {

	//Event handler for window close
	Window->Closed += ref new TypedEventHandler
		<CoreWindow^, CoreWindowEventArgs^>(this, &App::Closed);
	
	//Event handlers for detecting key presses
	Window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::KeyDown);
	Window->KeyUp += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::KeyUp);

	//Event handler for mouse actions
	Window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::PointerPressed);

	
}

void App::Load(String^ EntryPoint) {}

void App::Run() {
	
	//Initialise directx
	Game.Initialize();

	//set initial camera look angles
	//Game.UpdateLookAngleXZ(lookAngleXZ);
	//Game.UpdateLookAngleY(lookAngleY);
	
	// Obtain a pointer to the window
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();

	startPointerX = (Window->Bounds.Width) / 2; //set start position of mouse (x)
	startPointerY = (Window->Bounds.Height) / 2; // set start position of mouse (y)
	ResetPointerPosition(Window);

	// repeat until window closes
	while (!WindowClosed) {

		// Run ProcessEvents() to dispatch events
		Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

		Game.Update(wasd_keys);
		Game.Render();

		// calculate difference between last Pointer position and new position. Speed divider reduces this to a reasonable value in radians.
		float deltaX = (lastPointerX - Window->PointerPosition.X) / lookSpeedDivider;
		float deltaY = (lastPointerY - Window->PointerPosition.Y) / lookSpeedDivider;

		//Update camera look angles based on mouse movement
		Game.AdjustLookAngleXZ(deltaX); 
		Game.AdjustLookAngleY(deltaY);
		
		ResetPointerPosition(Window);

		//if space is pressed, make player jump
		if (spacePress == true) {
			Game.PlayerJump(wasd_keys);
		}
	}
}

void App::Uninitialize() {}

	// an "event" that is called when the application window is ready to be activated
void App::OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
	{
		//obtain pointer to the window
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();

		//Activate the window
		Window->Activate();
	}

void App::OnSuspending(Object^ Sender, SuspendingEventArgs^ Args) {

}
void App::OnResuming(Object^ Sender, Object^ Args) {

}

void App::Closed(CoreWindow^ sender, CoreWindowEventArgs^ args) {
	WindowClosed = true; //terminates endless loop
}

void App::PointerPressed(CoreWindow^ sender, PointerEventArgs^ args) {
	Game.FireArrow();
}

void App::ResetPointerPosition(CoreWindow^ Window) {

	Window->PointerPosition = Point(startPointerX, startPointerY); //reset pointer position
	lastPointerX = Window->PointerPosition.X; //for some reason, returns different value to startPointerX, so cannot simply set as startPointerX.
	lastPointerY = Window->PointerPosition.Y;
}

void App::KeyDown(CoreWindow^ sender, KeyEventArgs^ args) {

	//check which key is pressed down
	
	//WASD key downs
	if (args->VirtualKey == VirtualKey::W) {
		wasd_keys[0] = true;
	}
	if (args->VirtualKey == VirtualKey::A) {
		wasd_keys[1] = true;
	}
	if (args->VirtualKey == VirtualKey::S) {
		wasd_keys[2] = true;
	}
	if (args->VirtualKey == VirtualKey::D) {
		wasd_keys[3] = true;
	}
	
	//space key
	if (args->VirtualKey == VirtualKey::Space) {
		spacePress = true;
	}

	//if escape key, close
	if (args->VirtualKey == VirtualKey::Escape) {
		CoreApplication::Exit();
	}
}

void App::KeyUp(CoreWindow^ sender, KeyEventArgs^ args) {
	//check which key is pressed down
	if (args->VirtualKey == VirtualKey::W) {
		wasd_keys[0] = false;
	}
	if (args->VirtualKey == VirtualKey::A) {
		wasd_keys[1] = false;
	}
	if (args->VirtualKey == VirtualKey::S) {
		wasd_keys[2] = false;
	}
	if (args->VirtualKey == VirtualKey::D) {
		wasd_keys[3] = false;
	}

	if (args->VirtualKey == VirtualKey::Space) {
		spacePress = false;
	}
}

[MTAThread]    // define main() as a multi-threaded-apartment function

// the starting point of all programs
int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new AppSource());    // create and run a new AppSource class
	return 0;
}