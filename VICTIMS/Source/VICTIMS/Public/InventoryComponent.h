
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);					// �κ��丮 replicate

class UItemBase;

UENUM(BlueprintType)												// ������ �߰� ���� 
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult																			// ������ �߰� ��� (���� / ����)
{
	GENERATED_BODY()

	FItemAddResult() :									
	actualAmountAdded(0),
	operationResult(EItemAddResult::IAR_NoItemAdded),
	resultMessage(FText::GetEmpty()) {};

	int32 actualAmountAdded;							// ���� �߰��� ������ ����
	EItemAddResult operationResult;						// ������ �߰� ���
	FText resultMessage;								// ��� �޼���

	static FItemAddResult AddedNone(const FText& ErrorText)										// ������ �߰����� ���� - ����
	{
		FItemAddResult addedNoneResult;
		addedNoneResult.actualAmountAdded = 0;
		addedNoneResult.operationResult = EItemAddResult::IAR_NoItemAdded;
		addedNoneResult.resultMessage = ErrorText;
		return addedNoneResult;
	}
	static FItemAddResult AddedPartial(const int32 partialAmountAdded, const FText& ErrorText)	// ������ �Ϻθ� �߰��� - ����
	{
		FItemAddResult addedPartialResult;
		addedPartialResult.actualAmountAdded = partialAmountAdded;
		addedPartialResult.operationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		addedPartialResult.resultMessage = ErrorText;
		return addedPartialResult;
	}
	static FItemAddResult AddedAll(const int32 amountAdded, const FText& Message)				// ������ ��� �߰��� - ����
	{
		FItemAddResult addedAllResult;
		addedAllResult.actualAmountAdded = amountAdded;
		addedAllResult.operationResult = EItemAddResult::IAR_AllItemAdded;
		addedAllResult.resultMessage = Message;
		return addedAllResult;
	}
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnInventoryUpdated onInventoryUpdated;

//======================================================================================
// FUNCTIONS
//======================================================================================	

	UInventoryComponent();
	
	UFUNCTION(Category = "Inventory")											// ������ �߰� ����
	FItemAddResult HandleAddItem(UItemBase* inputItem);
	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* itemIn) const;							// ���������̺� �ִ� ���������� Ȯ��
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* itemIn) const;							// ID �� ������ ã��
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* itemIn) const;						// ���� ���� ã��
																				// ������ ����
	UFUNCTION(Category = "Inventory")										
	void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);						// ������ �ϳ� ����
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);		// ������ �������� ����
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);			// ���� ���� ������


	UFUNCTION(Category = "Inventory")															// �������� ���� ��������
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")															// �κ��丮 ���� ��������
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };	
	UFUNCTION(Category = "Inventory")															// ���� ������ ���� ����
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };


protected:

	UPROPERTY(EditInstanceOnly, Category="Inventory")								// ������ ���� ���� ����
	int32 InventorySlotsCapacity;
	UPROPERTY(VisibleAnywhere, Category="Inventory")								// ������ ���� ���� 
	TArray<TObjectPtr<UItemBase>> InventoryContents;

	//======================================================================================
	// FUNCTIONS
	//======================================================================================	

	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn);				// ���纸�� �ȵǴ� ������ ����
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);		// ���纸�� ������ ������ ����
	int32 CalculateNumberForFullStack(const UItemBase* StackableItem, int32 InitialRequestedAddAmount) const;	// ���纸�� ���� ���

	void AddNewItem(UItemBase* Item, const int32 AmountToAdd);				// ���ο� ������ �߰�

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
