
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);					// 인벤토리 replicate

class UItemBase;

UENUM(BlueprintType)												// 아이템 추가 여부 
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult																			// 아이템 추가 결과 (에러 / 성공)
{
	GENERATED_BODY()

	FItemAddResult() :									
	actualAmountAdded(0),
	operationResult(EItemAddResult::IAR_NoItemAdded),
	resultMessage(FText::GetEmpty()) {};

	int32 actualAmountAdded;							// 실제 추가된 아이템 개수
	EItemAddResult operationResult;						// 아이템 추가 결과
	FText resultMessage;								// 결과 메세지

	static FItemAddResult AddedNone(const FText& ErrorText)										// 아이템 추가되지 않음 - 에러
	{
		FItemAddResult addedNoneResult;
		addedNoneResult.actualAmountAdded = 0;
		addedNoneResult.operationResult = EItemAddResult::IAR_NoItemAdded;
		addedNoneResult.resultMessage = ErrorText;
		return addedNoneResult;
	}
	static FItemAddResult AddedPartial(const int32 partialAmountAdded, const FText& ErrorText)	// 아이템 일부만 추가됨 - 에러
	{
		FItemAddResult addedPartialResult;
		addedPartialResult.actualAmountAdded = partialAmountAdded;
		addedPartialResult.operationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		addedPartialResult.resultMessage = ErrorText;
		return addedPartialResult;
	}
	static FItemAddResult AddedAll(const int32 amountAdded, const FText& Message)				// 아이템 모두 추가됨 - 성공
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
	
	UFUNCTION(Category = "Inventory")											// 아이템 추가 관리
	FItemAddResult HandleAddItem(UItemBase* inputItem);
	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* itemIn) const;							// 데이터테이블에 있는 아이템인지 확인
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* itemIn) const;							// ID 로 아이템 찾기
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* itemIn) const;						// 보관 공간 찾기
																				// 아이템 제거
	UFUNCTION(Category = "Inventory")										
	void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);						// 아이템 하나 제거
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);		// 아이템 일정개수 제거
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);			// 보관 개수 나누기


	UFUNCTION(Category = "Inventory")															// 보관가능 개수 가져오기
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")															// 인벤토리 정보 가져오기
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };	
	UFUNCTION(Category = "Inventory")															// 보관 가능한 슬롯 세팅
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };


protected:

	UPROPERTY(EditInstanceOnly, Category="Inventory")								// 아이템 보관 가능 개수
	int32 InventorySlotsCapacity;
	UPROPERTY(VisibleAnywhere, Category="Inventory")								// 아이템 정보 모음 
	TArray<TObjectPtr<UItemBase>> InventoryContents;

	//======================================================================================
	// FUNCTIONS
	//======================================================================================	

	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn);				// 적재보관 안되는 아이템 관리
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);		// 적재보관 가능한 아이템 관리
	int32 CalculateNumberForFullStack(const UItemBase* StackableItem, int32 InitialRequestedAddAmount) const;	// 적재보관 개수 계산

	void AddNewItem(UItemBase* Item, const int32 AmountToAdd);				// 새로운 아이템 추가

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
