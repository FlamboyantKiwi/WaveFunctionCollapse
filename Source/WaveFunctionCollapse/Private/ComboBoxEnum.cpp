#include "ComboBoxEnum.h"
#include "UObject/Class.h" 

void UComboBoxEnum::SynchronizeProperties() {
	// Super must be called so standard ComboBox properties (colors, fonts, etc.) sync correctly
	Super::SynchronizeProperties();

	PopulateFromEnum();
}

void UComboBoxEnum::PopulateFromEnum()
{
	// Clear out any existing options first
	ClearOptions();

	if (TargetEnum.IsEmpty()) {
		return;
	}

	UEnum* FoundEnum = FindFirstObject<UEnum>(*TargetEnum);

	if (FoundEnum) {
		// Iterate through all enum values except the hidden "_MAX" value Unreal adds to the end
		for (int32 i = 0; i < FoundEnum->NumEnums() - 1; ++i) {
			// Skip any enum values explicitly marked with UMETA(Hidden)
			if (!FoundEnum->HasMetaData(TEXT("Hidden"), i)) {
				FText DisplayString = FoundEnum->GetDisplayNameTextByIndex(i);
				AddOption(DisplayString.ToString());
			}
		}

		// Optionally set the first item as the default selection so the box isn't blank
		if (GetOptionCount() > 0 && GetSelectedOption().IsEmpty()) {
			SetSelectedIndex(0);
		}
	} else {
		// Helpful debug text if you misspell the enum name in the UMG editor
		AddOption(TEXT("Invalid Enum Name"));
		SetSelectedIndex(0);
	}
}