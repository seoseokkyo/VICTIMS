
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
	//인벤토리에 추가된 항목의 실제 양
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded; // 우리가 원하는 필드에 실제 금액 추가

	//항목 추가 작업의 종료 상태 열거
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult operationResult;

	//결과 정보 메시지
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)// 새로운 항목추가 결과를 생성할 필요가 없을 때
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.operationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	}; 

	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText) // 일부 추가됨 
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.operationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message) // 전부 추가됨 
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

	// 아이템 추가 전, 인벤토리 한 칸에 여러개 적재가능한지 or 개당 한 칸을 사용하는 아이템인지 확인 
	// 여러수량 적재보관 가능하면 수량 추가 
	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItems* inputItem);

	// 아이템 드랍 or 팔기를 할 경우 해당 아이템이 인벤토리에 값이 실제로 들어가 있는지 확인 
	UFUNCTION(Category = "Inventory")
	UItems* FindMatchingItem(UItems* itemIn) const;

	
	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(UItems* itemToRemove); // 인벤토리 한 칸 사용하는 단일아이템 인벤토리에서 제거 

	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItems* itemToRemove, int32 desiredAmountToRemove); // 적재 보관하는 아이템에서 일정량만 제거 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const {return inventorySlotsCapacity;}; // 슬롯 개수 가져오기 -> UI 에서 빈 슬롯개수 쓸 때 사용 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItems*> GetInventoryContents() const {return inventoryContents;}; // 인벤토리에 있는 아이템 배열 가져오기 

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 newSlotsCapacity) {inventorySlotsCapacity = newSlotsCapacity;}; // 최대 슬롯개수 설정 

// 	void RefreshInventory(); // 네트워크에서 인벤토리 업데이트

protected:

	// PROPERTIES & VARIABLES
	//================================================================================

	UPROPERTY(EditInstanceOnly, Category = "Inventroy")
	int32 inventorySlotsCapacity; 	// 인벤토리 최대 슬롯 개수 

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<TObjectPtr<UItems>> inventoryContents; // 인벤토리에 담겨 있는 아이템 배열 목록

	
	
	// FUNCTIONS
	//================================================================================
	
	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItems* inputItem); // 인벤토리에 적재보관 불가한 아이템 처리
	int32 HandleStackableItems(UItems* inputItem, int32 requestedAddAmount); // 한 칸에 여러개 적재보관 가능한 아이템 처리 
	int32 CalculateNumberForFullStack(UItems* StackableItem, int32 InitialRequestedAddAmount); // 한 칸에 여러개 적재보관 가능한 아이템 -> 추가 적재 가능한 개수 계산

	
	void AddNewItem(UItems* item, const int32 amountToAdd); // 인벤토리에 없던 아이템 획득시 인벤토리에 새항목 추가
	
};
