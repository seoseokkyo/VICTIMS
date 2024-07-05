
#include "FItemStructure.h"

FItemStructure::FItemStructure()
{
	ID = "None";
	Icon = nullptr;
	Name = FText::FromString("None");
	Description = FText::FromString("None");
	ItemType = EItemType::Undefined;
	Quality = EItemQuality::Undefined;
	PriceValue = 0;
	IsStackable = false;
	MaxStackSize = 0;
	Class = nullptr;
	IsDroppable = false;
	SkeletalMesh = nullptr;
	WorldMesh = nullptr;
	Duration = 0.f;
	EquipmentSlot = EEquipmentSlot::Undefined;
	Strength = 0;
	Health = 0;
}
