
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItems;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() : 
	ActualAmountAdded(0),
	operationResult(EItemAddResult::IAR_NoItemAdded),
	ResultMessage(FText::GetEmpty())
	{}; 
	//�κ��丮�� �߰��� �׸��� ���� ��
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded; // �츮�� ���ϴ� �ʵ忡 ���� �ݾ� �߰�

	//�׸� �߰� �۾��� ���� ���� ����
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult operationResult;

	//��� ���� �޽���
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)// ���ο� �׸��߰� ����� ������ �ʿ䰡 ���� ��
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.operationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	}; 

	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText) // �Ϻ� �߰��� 
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.operationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message) // ���� �߰��� 
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.operationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};

	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ������ �߰� ��, �κ��丮 �� ĭ�� ������ ���簡������ or ���� �� ĭ�� ����ϴ� ���������� Ȯ�� 
	// �������� ���纸�� �����ϸ� ���� �߰� 
	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItems* inputItem);

	// ������ ��� or �ȱ⸦ �� ��� �ش� �������� �κ��丮�� ���� ������ �� �ִ��� Ȯ�� 
	UFUNCTION(Category = "Inventory")
	UItems* FindMatchingItem(UItems* itemIn) const;

	
	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(UItems* itemToRemove); // �κ��丮 �� ĭ ����ϴ� ���Ͼ����� �κ��丮���� ���� 

	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItems* itemToRemove, int32 desiredAmountToRemove); // ���� �����ϴ� �����ۿ��� �������� ���� 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const {return inventorySlotsCapacity;}; // ���� ���� �������� -> UI ���� �� ���԰��� �� �� ��� 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItems*> GetInventoryContents() const {return inventoryContents;}; // �κ��丮�� �ִ� ������ �迭 �������� 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 newSlotsCapacity) {inventorySlotsCapacity = newSlotsCapacity;}; // �ִ� ���԰��� ���� 

// 	void RefreshInventory(); // ��Ʈ��ũ���� �κ��丮 ������Ʈ

protected:

	// PROPERTIES & VARIABLES
	//================================================================================

	UPROPERTY(EditInstanceOnly, Category = "Inventroy")
	int32 inventorySlotsCapacity; 	// �κ��丮 �ִ� ���� ���� 

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<TObjectPtr<UItems>> inventoryContents; // �κ��丮�� ��� �ִ� ������ �迭 ���

	
	
	// FUNCTIONS
	//================================================================================
	
	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItems* inputItem); // �κ��丮�� ���纸�� �Ұ��� ������ ó��
	int32 HandleStackableItems(UItems* inputItem, int32 requestedAddAmount); // �� ĭ�� ������ ���纸�� ������ ������ ó�� 
	int32 CalculateNumberForFullStack(UItems* StackableItem, int32 InitialRequestedAddAmount); // �� ĭ�� ������ ���纸�� ������ ������ -> �߰� ���� ������ ���� ���

	
	void AddNewItem(UItems* item, const int32 amountToAdd); // �κ��丮�� ���� ������ ȹ��� �κ��丮�� ���׸� �߰�
	
};
