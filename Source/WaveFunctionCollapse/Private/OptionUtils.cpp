// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionUtils.h"


// --- Constructors ---

FOptionValue::FOptionValue()
    : DataType(EOptionDataType::Bool) {
}

FOptionValue::FOptionValue(bool InVal)
    : DataType(EOptionDataType::Bool), BoolValue(InVal) {
}

FOptionValue::FOptionValue(int32 InVal)
    : DataType(EOptionDataType::Int), IntValue(InVal) {
}

FOptionValue::FOptionValue(float InVal)
    : DataType(EOptionDataType::Float), FloatValue(InVal) {
}

FOptionValue::FOptionValue(const FString& InVal)
    : DataType(EOptionDataType::String), StringValue(InVal) {
}

FOptionValue::FOptionValue(const FText& InVal)
    : DataType(EOptionDataType::String), StringValue(InVal.ToString()) {
}

FOptionValue::FOptionValue(const FLinearColor& InVal)
    : DataType(EOptionDataType::Colour), ColourValue(InVal) {
}

FOptionValue::FOptionValue(const TArray<uint8>& InVal)
    : DataType(EOptionDataType::ByteArray), ByteArrayValue(InVal) {
}

// --- Conversion Methods ---

int32 FOptionValue::GetValueAsInt() const {
    switch (DataType) {
        case EOptionDataType::Bool:   return BoolValue ? 1 : 0;
        case EOptionDataType::Int:    return IntValue;
        case EOptionDataType::Float:  return static_cast<int32>(FloatValue);
        case EOptionDataType::String: return FCString::Atoi(*StringValue);
        default:                      return 0;
    }
}

float FOptionValue::GetValueAsFloat() const {
    switch (DataType) {
        case EOptionDataType::Bool:   return BoolValue ? 1.0f : 0.0f;
        case EOptionDataType::Int:    return static_cast<float>(IntValue);
        case EOptionDataType::Float:  return FloatValue;
        case EOptionDataType::String: return FCString::Atof(*StringValue); // Uses Atof for float parsing
        default:                      return 0.0f;
    }
}

bool FOptionValue::GetValueAsBool() const {
    switch (DataType) {
        case EOptionDataType::Bool:   return BoolValue;
        case EOptionDataType::Int:    return IntValue != 0;
        case EOptionDataType::Float:  return !FMath::IsNearlyZero(FloatValue);
        case EOptionDataType::String: return StringValue.ToBool() || FCString::Atoi(*StringValue) != 0;
        default:                      return false;
    }
}

FString FOptionValue::GetValueAsString() const {
    switch (DataType) {
    case EOptionDataType::Bool:       return BoolValue ? TEXT("True") : TEXT("False");
    case EOptionDataType::Int:        return FString::FromInt(IntValue);
    case EOptionDataType::Float:      return FString::SanitizeFloat(FloatValue);
    case EOptionDataType::String:     return StringValue;
    case EOptionDataType::Colour:     return ColourValue.ToString();
    default:                          return TEXT("");
    }
}

FLinearColor FOptionValue::GetValueAsColour() const {
    if (DataType == EOptionDataType::Colour) {
        return ColourValue;
    } else if (DataType == EOptionDataType::String) {
        // Unreal can automatically parse standard hex codes or format "(R=0,G=0,B=0,A=1)"
        FLinearColor OutColor;
        if (OutColor.InitFromString(StringValue)) {
            return OutColor;
        }
    }
    return FLinearColor::Black; // Default fallback
}

TArray<uint8> FOptionValue::GetValueAsByteArray() const {
    if (DataType == EOptionDataType::ByteArray) {
        return ByteArrayValue;
    }
    return TArray<uint8>(); // Return empty array if wrong type
}

void UOptionUtils::GetGridPositionForDirection(ENeighborDirection Direction, int32 & OutRow, int32 & OutColumn) {
    // If "None", place it in the center (Row 1, Col 1) as 'other content'
    if (Direction == ENeighborDirection::None) {
        OutRow = 1;
        OutColumn = 1;
        return;
    }

    // Get the exact offset we use in our WFC Generator
    FIntPoint Offset = FWfcMath::GetOffsetForDirection(Direction);

    // Convert the offset into 3x3 UI Grid coordinates
    OutColumn = 1 + Offset.X;
    OutRow = 1 - Offset.Y;
}

// --- Blueprint Library Wrappers ---
FOptionValue UOptionUtils::PackOption_Float(float Value) {
    return FOptionValue(Value); 
}

FOptionValue UOptionUtils::PackOption_Int(int32 Value) {
    return FOptionValue(Value); 
}

FOptionValue UOptionUtils::PackOption_Bool(bool Value) {
    return FOptionValue(Value); 
}

FOptionValue UOptionUtils::PackOption_String(const FString& Value) {
    return FOptionValue(Value); 
}

FOptionValue UOptionUtils::PackOption_Text(const FText& Value) {
    return FOptionValue(Value); 
}

FOptionValue UOptionUtils::PackOption_Colour(const FLinearColor& Value) {
    return FOptionValue(Value);
}

FOptionValue UOptionUtils::PackOption_ByteArray(const TArray<uint8>& Value) {
    return FOptionValue(Value);
}

FOptionValue UOptionUtils::PackOption_NeighborArray(const TArray<ENeighborDirection>& Directions) {
    TArray<uint8> Bytes;

    // Loop through the enums and cast them to bytes
    for (ENeighborDirection Dir : Directions) {
        Bytes.Add(static_cast<uint8>(Dir));
    }

    // Pack the resulting byte array into your FOptionValue
    return FOptionValue(Bytes);
}

int32 UOptionUtils::Conv_OptionValueToInt(const FOptionValue& Option) {
    return Option.GetValueAsInt(); 
}

float UOptionUtils::Conv_OptionValueToFloat(const FOptionValue& Option) {
    return Option.GetValueAsFloat(); 
}

bool UOptionUtils::Conv_OptionValueToBool(const FOptionValue& Option) {
    return Option.GetValueAsBool(); 
}

FString UOptionUtils::Conv_OptionValueToString(const FOptionValue& Option) {
    return Option.GetValueAsString(); 
}

FLinearColor UOptionUtils::Conv_OptionValueToColour(const FOptionValue& Option) {
    return Option.GetValueAsColour();
}

TArray<uint8> UOptionUtils::Conv_OptionValueToByteArray(const FOptionValue& Option) {
    return Option.GetValueAsByteArray();
}

TArray<ENeighborDirection> UOptionUtils::Conv_OptionValueToNeighborArray(const FOptionValue& Option) {
    TArray<ENeighborDirection> ResultArray;
    TArray<uint8> Bytes = Option.GetValueAsByteArray();

    // Loop through the bytes and cast them to the enum
    for (uint8 ByteVal : Bytes) {
        ResultArray.Add(static_cast<ENeighborDirection>(ByteVal));
    }

    return ResultArray;
}