#pragma once

#include "DataHandler.hpp"

#include "Core/Main.hpp"

namespace Events
{
	using namespace ModData;

	class MainEvent
	{
	public:
		// Initialization of hooks and template functions
		static void InstallHooks()
		{
			REL::Relocation<std::uintptr_t> arrowUpdateVtbl{ RE::ArrowProjectile::VTABLE[0] };
			_ArrowUpdate = arrowUpdateVtbl.write_vfunc(0xAB, OnArrowUpdateTemplate);
			logger::info("ArrowUpdate hooked at virtual table index 0xAB. Address: 0x{:X}", _ArrowUpdate.address());
		}

	private:

		static void OnArrowUpdateTemplate(RE::ArrowProjectile* a_this, float a_delta)
		{
			ModCore::Main::Process(a_this);

			_ArrowUpdate(a_this, a_delta);
		}
		static inline REL::Relocation<decltype(OnArrowUpdateTemplate)> _ArrowUpdate;
	};
};
