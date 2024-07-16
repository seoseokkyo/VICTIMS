#pragma once

// enum class EUsingSlotType
// {
// 	NativeFromInventory = 0,
// 	NativeFromContainer,
// 	NativeFromShop,
// 	EUsingSlotTypeMax
// };

struct FReturnTupleBoolInt
{
	bool Success = true; int8 Index = -1;
};

struct FReturnTupleSlotNativeID {
// 	//bool NativeFromInventory = false;
// 	//bool NativeFromContainer = false;
// 	//bool NativeFromShop = false;
// 	EUsingSlotType usingType = EUsingSlotType::EUsingSlotTypeMax;

	uint8 Index = 0;
};