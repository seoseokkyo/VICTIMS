
#include "WindowLayout.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "VictimsGameInstance.h"
#include "DragWidget.h"
#include "MyHUD.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "Sound/SoundBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

void UWindowLayout::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddUniqueDynamic(this, &UWindowLayout::OnButtonQuitClicked);

	PlayerController = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	GameInstance = Cast<UVictimsGameInstance>(GetGameInstance());

	WindowLayout = ELayout::Undefined;

	SetVisibility(ESlateVisibility::Hidden);
}

FReply UWindowLayout::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UWindowLayout::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return CustomDetectDrag(InMouseEvent, this, EKeys::LeftMouseButton);
}

void UWindowLayout::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UDragWidget* DragDropOperation = NewObject<UDragWidget>();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	DragDropOperation->WidgetReference = this;
	DragDropOperation->DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	DragDropOperation->DefaultDragVisual = this;
	DragDropOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragDropOperation;
}

void UWindowLayout::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation))
	{
		if (DragWidgetResult->WidgetReference->GetName() == GetName())
		{
			RemoveFromParent();
		}
	}
}

bool UWindowLayout::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Success = false;
	return Success;
}

void UWindowLayout::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	if (UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation))
	{
		if (DragWidgetResult->WidgetReference->GetName() != GetName())
		{
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
void UWindowLayout::ToggleWindow()
{
	FString strName;
	GetName(strName);

	bool bvisibleCheck = (GetVisibility() == ESlateVisibility::Hidden);
	if (ToggleSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ToggleSound);
	}
	if (false == strName.IsEmpty())
	{
// 		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Toggle%s Called : %s"), *strName, bvisibleCheck ? TEXT("On") : TEXT("Off")));
	}	

	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
void UWindowLayout::OnButtonQuitClicked()
{
	if (IsValid(PlayerController))
	{
		PlayerController->HUD_Reference->ToggleWindow(WindowLayout);
		PlayerController->SetInputDependingFromVisibleWidgets();
		if (ClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
		}
	}
}

void UWindowLayout::SetTitleToWindow(FText Text)
{
	WindowTitle->SetText(Text);
}

FEventReply UWindowLayout::RedirectMouseDownToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return Reply;
}

FReply UWindowLayout::CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey)
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
	if ((InMouseEvent.GetEffectingButton() == DragKey && TopBorder->IsHovered()) || InMouseEvent.IsTouchEvent())
	{
		FEventReply Reply;
		Reply.NativeReply = FReply::Handled();

		if (WidgetDetectingDrag)
		{
			TSharedPtr<SWidget> SlateWidgetDetectingDrag = WidgetDetectingDrag->GetCachedWidget();
			if (SlateWidgetDetectingDrag.IsValid())
			{
				Reply.NativeReply = Reply.NativeReply.DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), DragKey);
				return Reply.NativeReply;
			}
		}
	}

	return FReply::Unhandled();
}
