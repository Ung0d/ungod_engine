#include <boost/test/unit_test.hpp>
#include "ungod/AI/Predictor.h"

BOOST_AUTO_TEST_SUITE(AITest)

BOOST_AUTO_TEST_CASE( predictor_test )
{
	ungod::AI::Model model;
	model.load("test_data/converted_model.tflite");
	BOOST_REQUIRE(model.isLoaded());
	ungod::AI::Predictor pred;
	BOOST_REQUIRE(pred.setModel(model));
}

BOOST_AUTO_TEST_SUITE_END()
