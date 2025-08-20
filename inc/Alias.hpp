#pragma once

namespace sqrp
{
	class Command;
	class Device;
	class Swapchain;

	using CommandHandle = std::shared_ptr<Command>;
	using DeviceHandle = std::shared_ptr<Device>;
}