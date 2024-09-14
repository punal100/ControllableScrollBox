// Copyright Epic Games, Inc. All Rights Reserved.

#include "Punal_ScrollBoxSlot.h"
#include "Components/Widget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Punal_ScrollBoxSlot)

/////////////////////////////////////////////////////
// UPunal_ScrollBoxSlot

UPunal_ScrollBoxSlot::UPunal_ScrollBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	Size = FSlateChildSize(ESlateSizeRule::Automatic);
}

void UPunal_ScrollBoxSlot::BuildSlot(TSharedRef<SPunal_ScrollBox> ScrollBox)
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	ScrollBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.Expose(Slot)
		.SizeParam(UWidget::ConvertSerializedSizeParamToRuntime(Size))
	[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
FMargin UPunal_ScrollBoxSlot::GetPadding() const
{
	return Slot ? Slot->GetPadding() : Padding;
}

void UPunal_ScrollBoxSlot::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( Slot )
	{
		Slot->SetPadding(InPadding);
	}
}

FSlateChildSize UPunal_ScrollBoxSlot::GetSize() const
{
	return Size;
}

void UPunal_ScrollBoxSlot::SetSize(FSlateChildSize InSize)
{
	Size = InSize;
	if (Slot)
	{
		Slot->SetSizeParam(UWidget::ConvertSerializedSizeParamToRuntime(InSize));
	}
}

EHorizontalAlignment UPunal_ScrollBoxSlot::GetHorizontalAlignment() const
{
	return Slot ? Slot->GetHorizontalAlignment() : HorizontalAlignment.GetValue();
}

void UPunal_ScrollBoxSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( Slot )
	{
		Slot->SetHorizontalAlignment(InHorizontalAlignment);
	}
}

EVerticalAlignment UPunal_ScrollBoxSlot::GetVerticalAlignment() const
{
	return Slot ? Slot->GetVerticalAlignment() : VerticalAlignment.GetValue();
}

void UPunal_ScrollBoxSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if (Slot)
	{
		Slot->SetVerticalAlignment(InVerticalAlignment);
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

void UPunal_ScrollBoxSlot::SynchronizeProperties()
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	SetPadding(Padding);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	SetSize(Size);
}

void UPunal_ScrollBoxSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}

