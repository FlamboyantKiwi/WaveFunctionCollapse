// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WFCTypes.h"
#include "OptionUtils.generated.h"

UENUM(BlueprintType)
enum class EOptionDataType : uint8
{
    Bool    UMETA(DisplayName = "Bool"),
    Int     UMETA(DisplayName = "Integer"),
    Float   UMETA(DisplayName = "Float"),
    String  UMETA(DisplayName = "String"),
    Colour   UMETA(DisplayName = "Colour"),
	ByteArray UMETA(DisplayName = "Byte Array") 
};

USTRUCT(BlueprintType)
struct FOptionValue {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    EOptionDataType DataType = EOptionDataType::Bool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    bool BoolValue = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    int32 IntValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    float FloatValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    FString StringValue = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
    FLinearColor ColourValue = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option Value")
	TArray<uint8> ByteArrayValue;

    // Constructors
    FOptionValue();
    FOptionValue(bool InVal);
    FOptionValue(int32 InVal);
    FOptionValue(float InVal);
    FOptionValue(const FString& InVal);
    FOptionValue(const FText& InVal);
    FOptionValue(const FLinearColor& InVal);
	FOptionValue(const TArray<uint8>& InVal);

    // Conversion Methods
    int32 GetValueAsInt() const;
    float GetValueAsFloat() const;
    bool GetValueAsBool() const;
    FString GetValueAsString() const;
    FLinearColor GetValueAsColour() const;
	TArray<uint8> GetValueAsByteArray() const;
};



UCLASS()
class WAVEFUNCTIONCOLLAPSE_API UOptionUtils : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    // Converts a neighbor direction into 3x3 UI Grid coordinates.
    // 'None' returns the center (Row 1, Column 1).
    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Get Grid Position", CompactNodeTitle = "GridPos"))
    static void GetGridPositionForDirection(ENeighborDirection Direction, int32& OutRow, int32& OutColumn);

	//  Pack Functions
    // Note: CompactNodeTitle: shrinks the Blueprints node, BlueprintAutocast enables auto-wiring
    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Float)", CompactNodeTitle = "Float->Opt", BlueprintAutocast))
    static FOptionValue PackOption_Float(float Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Int)", CompactNodeTitle = "Int->Opt", BlueprintAutocast))
    static FOptionValue PackOption_Int(int32 Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Bool)", CompactNodeTitle = "Bool->Opt", BlueprintAutocast))
    static FOptionValue PackOption_Bool(bool Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (String)", CompactNodeTitle = "Str->Opt", BlueprintAutocast))
    static FOptionValue PackOption_String(const FString& Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Text)", CompactNodeTitle = "Txt->Opt", BlueprintAutocast))
    static FOptionValue PackOption_Text(const FText& Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Colour)", CompactNodeTitle = "Col->Opt", BlueprintAutocast))
    static FOptionValue PackOption_Colour(const FLinearColor& Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (ByteArray)", CompactNodeTitle = "ByteArray->Opt", BlueprintAutocast))
    static FOptionValue PackOption_ByteArray(const TArray<uint8>& Value);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "Pack Option (Directions)", CompactNodeTitle = "Directions->Opt", BlueprintAutocast))
    static FOptionValue PackOption_NeighborArray(const TArray<ENeighborDirection>& Directions);

    //  Unpack / Convert Functions
    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As Integer", CompactNodeTitle = "->Int", BlueprintAutocast))
    static int32 Conv_OptionValueToInt(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As Float", CompactNodeTitle = "->Float", BlueprintAutocast))
    static float Conv_OptionValueToFloat(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As Bool", CompactNodeTitle = "->Bool", BlueprintAutocast))
    static bool Conv_OptionValueToBool(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As String", CompactNodeTitle = "->Str", BlueprintAutocast))
    static FString Conv_OptionValueToString(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As Colour", CompactNodeTitle = "->Col", BlueprintAutocast))
    static FLinearColor Conv_OptionValueToColour(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As ByteArray", CompactNodeTitle = "->ByteArray", BlueprintAutocast))
    static TArray<uint8> Conv_OptionValueToByteArray(const FOptionValue& Option);

    UFUNCTION(BlueprintPure, Category = "Options", meta = (DisplayName = "As Direction", CompactNodeTitle = "->Direction", BlueprintAutocast))
    static TArray<ENeighborDirection> Conv_OptionValueToNeighborArray(const FOptionValue& Option);
};