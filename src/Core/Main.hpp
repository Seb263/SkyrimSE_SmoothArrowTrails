#pragma once

#include "DataHandler.hpp"
#include "SettingsIni.hpp"

#include "Utils/MiscUtils.hpp"
#include "Utils/NiUtils.hpp"

namespace ModCore
{
	class Main
	{
	public:

		static void Process(RE::ArrowProjectile* a_this)
		{
			if (!a_this || !a_this->Is3DLoaded() || !SettingsIni::bEnabled) return;
			
			const auto& runtime = a_this->GetProjectileRuntimeData();
			if (runtime.flags.any(RE::Projectile::Flags::kProcessedImpacts)) return;

			auto* tracerRoot = a_this->GetNodeByName("TracerRoot");
			if (!tracerRoot) return;
			
			if (!CheckInitialized(tracerRoot)) return;

			ApplyTracerScale(tracerRoot, runtime.distanceMoved);
		}

	private:

		static bool CheckInitialized(RE::NiAVObject* a_tracerRoot)
		{
			const auto initData = NiUtils::GetExtraDataValue<RE::NiIntegerExtraData>(a_tracerRoot, "initialized");
			if (!initData) {
				if (a_tracerRoot->GetAppCulled() || a_tracerRoot->local.scale < 0.2f) {
					NiUtils::StoreExtraData<RE::NiIntegerExtraData>(a_tracerRoot, "initialized", -1);
					return false;
				}
				NiUtils::StoreExtraData<RE::NiIntegerExtraData>(a_tracerRoot, "initialized", 1);
				return true;
			}
			return initData.value_or(-1) != -1;
		}

		static void ApplyTracerScale(RE::NiAVObject* a_tracerRoot, float a_distanceMoved)
		{
			const float ratio = (a_distanceMoved - SettingsIni::fTracerDistanceMin) / (SettingsIni::fTracerDistanceMax - SettingsIni::fTracerDistanceMin);
			const float t = std::clamp(ratio, 0.0f, 1.0f);
			const float eased = 1.0f - (1.0f - t) * (1.0f - t);
			const float targetScale = std::lerp(SettingsIni::fTracerScaleMin, SettingsIni::fTracerScaleMax, eased);

			a_tracerRoot->local.scale = targetScale;
			NiUtils::UpdateObject(a_tracerRoot);
		}
	};
};
