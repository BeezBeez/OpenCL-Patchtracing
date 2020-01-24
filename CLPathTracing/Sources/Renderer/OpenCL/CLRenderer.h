#pragma once
#include "Core/Core.h"

namespace PathTracer
{
	class CLRenderer
	{
	public:
		Platform RendererPlatform;
		Device RendererDevice;
		
	public:
		CLRenderer() {}
		~CLRenderer() {}

		void InitRenderer()
		{
			this->SelectPlatform();
			ConsoleOutput << "\nUsing OpenCL Platform : " << this->RendererPlatform.getInfo<CL_PLATFORM_NAME>() << endl << endl;

			this->SelectDevice();
			ConsoleOutput << "\nUsing OpenCL Device: " << this->RendererDevice.getInfo<CL_DEVICE_NAME>() << endl << endl;
			this->PrintDeviceInfo();
		}

		void SelectPlatform()
		{
			//List all available OpenCL platforms
			vector<Platform> Platforms;
			Platform::get(&Platforms);

			//Print OpenCL platforms name
			ConsoleOutput << "Available OpenCL Platforms : \n\n";
			for (unsigned int Idx = 0; Idx < Platforms.size(); Idx++) {
				ConsoleOutput << "\t" << Idx + 1 << " : " << Platforms[Idx].getInfo<CL_PLATFORM_NAME>() << endl;
			}

			unsigned int PlatformID = 1;
			if (Platforms.size() > 1) //Ask to the user which platform to use only if they really have the choice
			{
				//Select and create an OpenCL platform
				ConsoleOutput << endl << "Type the ID of the OpenCL Platform you want to use > ";
				ConsoleInput >> PlatformID;

				//Prevent invalid user input
				while (PlatformID < 1 || PlatformID > Platforms.size())
				{
					ConsoleInput.clear();
					ConsoleInput.ignore(ConsoleInput.rdbuf()->in_avail(), '\n');

					ConsoleOutput << "ERROR : No such platform !\n";
					ConsoleOutput << "Type the ID of the OpenCL Platform you want to use > ";

					ConsoleInput >> PlatformID;
				}
			}


			RendererPlatform = Platforms[PlatformID - 1];
		}

		void SelectDevice()
		{
			vector<Device> Devices;
			RendererPlatform.getDevices(CL_DEVICE_TYPE_ALL, &Devices);

			ConsoleOutput << "Available OpenCL Devices for the selected platform : \n\n";
			for (unsigned int Idx = 0; Idx < Devices.size(); Idx++){
				ConsoleOutput << "\t" << Idx + 1 << " : " << Devices[Idx].getInfo<CL_DEVICE_NAME>() << endl;
			}

			unsigned int DeviceID = 1;
			if (Devices.size() > 1) //Ask to the user which device to use only if they really have the choice
			{
				ConsoleOutput << endl << "Type the ID of the Device you want to use > ";
				ConsoleInput >> DeviceID;

				while (DeviceID < 1 || DeviceID > Devices.size())
				{
					ConsoleInput.clear();
					ConsoleInput.ignore(ConsoleInput.rdbuf()->in_avail(), '\n');

					ConsoleOutput << "ERROR : No such device !\n";
					ConsoleOutput << "Type the ID of the Device you want to use > ";

					ConsoleInput >> DeviceID;
				}
			}

			RendererDevice = Devices[DeviceID - 1];
		}

		void PrintDeviceInfo()
		{
			ConsoleOutput << "OpenCL Device : " << RendererDevice.getInfo<CL_DEVICE_NAME>() << endl;
			ConsoleOutput << "Max compute units : " << RendererDevice.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
			ConsoleOutput << "Max work group size : " << RendererDevice.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << endl;
			ConsoleOutput << "Max clock frequency : " << RendererDevice.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << endl;
		}
	};
}