
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ELayout.h"
#include "WindowLayout.generated.h"

class AVICTIMSPlayerController;
class UVictimsGameInstance;
UCLASS()
class VICTIMS_API UWindowLayout : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	virtual void ToggleWindow();

	UPROPERTY()
	class AVICTIMSPlayerController* PlayerController;
	UPROPERTY()
	class UVictimsGameInstance* GameInstance;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WindowTitle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ELayout WindowLayout;

	UFUNCTION()
	virtual void OnButtonQuitClicked();
	
	UPROPERTY(meta = (BindWidget))	
	class UBorder* TopBorder;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	virtual void SetTitleToWindow(FText Text);
	
	virtual FEventReply RedirectMouseDownToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	FReply CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey);
	
//======================================================================================================================
// UI Sound

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* ToggleSound;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* ClickSound;

private:
	UPROPERTY()
	class UDragWidget* DragWidget;
	
	FVector2D DragOffset;
};
