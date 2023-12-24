bool CInterfaceSystem::Initialize()
{
	DumpInterfaces();
	return true;
}

void CInterfaceSystem::DumpInterfaces()
{
	for (const std::string& module : modules)
	{
		HMODULE moduleHandle = GetModuleHandleSafe(module.c_str());
		if (!moduleHandle)
		{
			std::cout << "Failed to get module handle for: " << module << std::endl;
			continue;
		}

		std::uint8_t* create_interface = reinterpret_cast<std::uint8_t*>(GetProcAddress(moduleHandle, "CreateInterface"));
		if (!create_interface)
		{
			std::cout << "Failed to get CreateInterface function address for: " << module << std::endl;
			continue;
		}

		using interface_callback_fn = void* (__cdecl*)();

		typedef struct _interface_reg_t
		{
			interface_callback_fn callback;
			const char* name;
			_interface_reg_t* flink;
		} interface_reg_t;

		interface_reg_t* interface_list = *reinterpret_cast<interface_reg_t**>(gPatternScaningSystem->ResolveRip(create_interface, 3, 7));

		if (!interface_list)
		{
			std::cout << "Interface list not found for: " << module << std::endl;
			continue;
		}

		for (interface_reg_t* it = interface_list; it; it = it->flink)
		{
			std::cout << module.c_str() << "->" << it->name << "->" << it->callback << "\n";
		}
	}
}


HMODULE CInterfaceSystem::GetModuleHandleSafe(const char* moduleName)
{
	HMODULE moduleHandle = GetModuleHandleA(moduleName);
	if (moduleHandle == nullptr) {
		return nullptr;
	}
	return moduleHandle;
}
