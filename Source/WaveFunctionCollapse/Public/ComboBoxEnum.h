#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "OptionUtils.h"
#include "ComboBoxEnum.generated.h"

/* A custom ComboBoxString that automatically populates itself based on a provided UENUM. */
UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONCOLLAPSE_API UComboBoxEnum : public UComboBoxString {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnumComboBox")
	FString TargetEnum;

	UFUNCTION(BlueprintCallable, Category = "Options")
	FOptionValue GetOptionValue() const;

	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetOptionValue(const FOptionValue& InValue);
protected:
	// Called when properties are changed in the UMG designer, and at runtime setup
	virtual void SynchronizeProperties() override;

private:
	// A helper function to keep population logic clean
	void PopulateFromEnum();
};

