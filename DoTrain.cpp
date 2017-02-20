#include <iostream>
#include "NeuralNetwork.h"

int main(int argc, char **argv)
{
	Parameter* userSetting = new Parameter(argc, argv);

	NeuralNetwork::TrainModel(userSetting);

	delete userSetting;
	return 0;
}