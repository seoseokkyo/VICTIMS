
#include "MyHUD.h"
#include "FWidgetsLayoutBP.h"
#include "blueprint/UserWidget.h"
#include "ContainerLayout.h"
#include "HUDLayout.h"
#include "InteractiveText_Panel.h"
#include "ELayout.h"
#include "InventoryLayout.h"
#include "MainLayout.h"
#include "ProfileLayout.h"
#include "InventoryManagerComponent.h"
#include "AVICTIMSPlayerController.h"

AMyHUD::AMyHUD()
{
}

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (auto ownerCon = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		if (false == ownerCon->IsLocalController())
		{
			return;
		}
	}

	UDataTable* BP_WidgetDB = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/Widgets_DB.Widgets_DB'"));
	if (IsValid(BP_WidgetDB))
	{
		WidgetDB = BP_WidgetDB;
	}

	const UDataTable* WidgetTable = WidgetDB;
	FWidgetsLayoutBP* NewWidgetData = nullptr;

	NewWidgetData = WidgetTable->FindRow<FWidgetsLayoutBP>(FName("HUDLayout_WBP"), "", true);
	if (NewWidgetData)
	{
		HUDReference = CreateWidget<UHUDLayout>(GetWorld(), NewWidgetData->Widget);

		if (HUDReference)
		{
			HUDReference->AddToViewport();
		}
	}

	if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		PlayerController->SetupHUDReferences();
	}
}

bool AMyHUD::IsAnyWidgetVisible()
{
	if (
		HUDReference->MainLayout->Inventory->IsVisible()
		||
		HUDReference->MainLayout->Profile->IsVisible()
		||
		HUDReference->MainLayout->Container->IsVisible()
		)
	{
		return true;
	}
	return false;
}

void AMyHUD::ToggleWindow(const ELayout Layout)
{
	if (Layout == ELayout::Inventory)
	{
		if (HUDReference->MainLayout->Inventory)
		{
			HUDReference->MainLayout->Inventory->ToggleWindow();
		}
	}
	else if (Layout == ELayout::Equipment)
	{
		if (HUDReference->MainLayout->Profile)
		{
			HUDReference->MainLayout->Profile->ToggleWindow();
		}
	}
	else if (Layout == ELayout::Container)
	{
		if (HUDReference->MainLayout->Container)
		{
			HUDReference->MainLayout->Container->ToggleWindow();
		}
	}
}

FWidgetsLayoutBP* AMyHUD::GetWidgetBPClass(FName Name)
{
	const UDataTable* WidgetTable = WidgetDB;

	if (!IsValid(WidgetTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("WidgetTable Not Valid"))
	}
	return WidgetTable->FindRow<FWidgetsLayoutBP>(Name, "", true);
}

UUserWidget* AMyHUD::GenerateInteractWidget(FText Text)
{
	const UDataTable* WidgetTable = WidgetDB;
	FWidgetsLayoutBP* NewWidgetData = nullptr;

	UUserWidget* EntryUserWidget = CreateWidgetFromDataTable(WidgetTable, NewWidgetData, FName("InteractiveText_Entry_WBP"));
	UInteractiveText_Entry* Entry = Cast<UInteractiveText_Entry>(EntryUserWidget);

	UUserWidget* PanelUserWidget = CreateWidgetFromDataTable(WidgetTable, NewWidgetData, FName("InteractiveText_Panel_WBP"));
	UInteractiveText_Panel* Panel = Cast<UInteractiveText_Panel>(PanelUserWidget);
	Panel->AddEntryToList(Entry);

	return Panel;
}

UUserWidget* AMyHUD::CustomCreateWidget(FName Name)
{
	const UDataTable* WidgetTable = WidgetDB;
	FWidgetsLayoutBP* NewWidgetData = nullptr;

	UUserWidget* UserWidget = CreateWidgetFromDataTable(WidgetTable, NewWidgetData, Name);
	if (UserWidget)
	{
		return UserWidget;
	}
	return nullptr;
}

UUserWidget* AMyHUD::CreateWidgetFromDataTable(const UDataTable* WidgetTable, FWidgetsLayoutBP*& NewWidgetData, FName Name)
{
	NewWidgetData = WidgetTable->FindRow<FWidgetsLayoutBP>(Name, "", true);
	if (NewWidgetData)
	{
		return CreateWidget<UUserWidget>(GetWorld(), NewWidgetData->Widget);
	}
	return nullptr;
}
