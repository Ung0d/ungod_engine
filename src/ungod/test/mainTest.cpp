#define BOOST_TEST_MODULE UngodBaseTesting
#include <boost/test/unit_test.hpp>
#include "ungod/application/Application.h"
#include "ungod/test/mainTest.h"

std::unique_ptr<ungod::Application> EmbeddedTestApp::embeddedApp;

BOOST_GLOBAL_FIXTURE( EmbeddedTestApp );
