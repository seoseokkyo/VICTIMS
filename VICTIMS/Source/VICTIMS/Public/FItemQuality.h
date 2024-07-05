// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FItemQuality.generated.h"

USTRUCT()
struct FItemQuality
{
	GENERATED_BODY()

	static FLinearColor Undefined;
	static FLinearColor Common;
	static FLinearColor UnCommon;
	static FLinearColor Rare;
	static FLinearColor Epic;
	static FLinearColor Legendary;
};