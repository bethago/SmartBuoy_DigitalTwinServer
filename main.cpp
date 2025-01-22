#include "main.h"
#include "config.h"
#include "oneM2M.h"
#include <iostream>
#include <string>
#include <csignal>

void signalHandler(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	exit(signum);
}

int main()
{
	try
	{
		signal(SIGINT, signalHandler);

		int input;
		std::cout << "Enter the number below for your specific task\n";
		std::cout << "1 : Run Digital Twin Server in Realtime\n";
		std::cout << "2 : Run Simulation Based on Log File" << std::endl;
		std::cin >> input;
		while (input != 1 && input != 2)
		{
			std::cout << "Enter the number below for your specific task\n";
			std::cout << "1 : Run Digital Twin Server in Realtime\n";
			std::cout << "2 : Run Simulation Based on Log File" << std::endl;
			std::cin >> input;
		}

		if (input == 1)
		{
			// Run Digital Twin Server in Realtime
			std::string cseUrl = "http://"+CSE_SERVER_IP":"+CSE_SERVER_PORT;
			std::string notificationUrl = "http://"+DT_SERVER_IP+":"+DT_SERVER_PORT+"/notify";

			OneM2M oneM2M(cseUrl, notificationUrl);;
			oneM2M.createSubscription("", "")
			oneM2M.startNotificationServer()
		}
		else if (input == 2)
		{
			// Run Simulation Based on Log File
		}



	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    std::cout << "Hello World!\n";

	return 0;
}