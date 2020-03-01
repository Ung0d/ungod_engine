#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include "ungod/application/Application.h"
#include "tensorflow/lite/model.h"

//wrap a global ungod::Application object that is constructed for all tests and that sets up all neccessary stuff
struct EmbeddedTestApp
{
	EmbeddedTestApp()
	{
		embeddedApp = new ungod::Application();
		embeddedApp->initApplication();
        ungod::Logger::info("Test application object constructed...");
        ungod::Logger::endl();	
    }
	~EmbeddedTestApp()
    {
		delete embeddedApp;
        ungod::Logger::info("Test application object destroyed...");
        ungod::Logger::endl();
    }

    static ungod::Application& getApp() { return *embeddedApp; }

private:
    static ungod::Application* embeddedApp;
};

#endif // MAIN_TEST_H
