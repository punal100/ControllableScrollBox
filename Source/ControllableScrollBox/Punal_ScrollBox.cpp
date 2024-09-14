// Code Written By Punal Manalan

#include "Punal_ScrollBox.h"
#include "SPunal_SCrollbox.h"
#include "Punal_ScrollBoxSlot.h"
#include "UObject/EditorObjectVersion.h"
#include "Styling/UMGCoreStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Punal_ScrollBox)

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UPunal_ScrollBox

static FScrollBoxStyle* DefaultScrollBoxStyle = nullptr;
static FScrollBarStyle* DefaultScrollBoxBarStyle = nullptr;

#if WITH_EDITOR
static FScrollBoxStyle* EditorScrollBoxStyle = nullptr;
static FScrollBarStyle* EditorScrollBoxBarStyle = nullptr;
#endif 

UPunal_ScrollBox::UPunal_ScrollBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Orientation(Orient_Vertical)
	, ScrollBarVisibility(ESlateVisibility::Visible)
	, ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	, ScrollbarThickness(9.0f, 9.0f)
	, ScrollbarPadding(2.0f)
	, AlwaysShowScrollbar(false)
	, AlwaysShowScrollbarTrack(false)
	, AllowOverscroll(true)
	, BackPadScrolling(false)
	, FrontPadScrolling(false)
	, NavigationDestination(EDescendantScrollDestination::IntoView)
	, NavigationScrollPadding(0.0f)
	, ScrollWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
{
	bIsVariable = false;

	SetVisibilityInternal(ESlateVisibility::Visible);
	SetClipping(EWidgetClipping::ClipToBounds);

	if (DefaultScrollBoxStyle == nullptr)
	{
		DefaultScrollBoxStyle = new FScrollBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox"));

		// Unlink UMG default colors.
		DefaultScrollBoxStyle->UnlinkColors();
	}

	if (DefaultScrollBoxBarStyle == nullptr)
	{
		DefaultScrollBoxBarStyle = new FScrollBarStyle(FUMGCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar"));

		// Unlink UMG default colors.
		DefaultScrollBoxBarStyle->UnlinkColors();
	}
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		WidgetStyle = *DefaultScrollBoxStyle;
	WidgetBarStyle = *DefaultScrollBoxBarStyle;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITOR 
		if (EditorScrollBoxStyle == nullptr)
		{
			EditorScrollBoxStyle = new FScrollBoxStyle(FCoreStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox"));

			// Unlink UMG Editor colors from the editor settings colors.
			EditorScrollBoxStyle->UnlinkColors();
		}

	if (EditorScrollBoxBarStyle == nullptr)
	{
		EditorScrollBoxBarStyle = new FScrollBarStyle(FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorScrollBoxBarStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			WidgetStyle = *EditorScrollBoxStyle;
		WidgetBarStyle = *EditorScrollBoxBarStyle;
		PRAGMA_ENABLE_DEPRECATION_WARNINGS

			// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
			PostEditChange();
	}
#endif // WITH_EDITOR
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		bAllowRightClickDragScrolling = true;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UPunal_ScrollBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyScrollBox.Reset();
}

UClass* UPunal_ScrollBox::GetSlotClass() const
{
	return UPunal_ScrollBoxSlot::StaticClass();
}

void UPunal_ScrollBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyScrollBox.IsValid())
	{
		CastChecked<UPunal_ScrollBoxSlot>(InSlot)->BuildSlot(MyScrollBox.ToSharedRef());
	}
}

void UPunal_ScrollBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyScrollBox.IsValid() && InSlot->Content)
	{
		const TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyScrollBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedRef<SWidget> UPunal_ScrollBox::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyScrollBox = SNew(SPunal_ScrollBox)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.ScrollWhenFocusChanges(ScrollWhenFocusChanges)
		.BackPadScrolling(BackPadScrolling)
		.FrontPadScrolling(FrontPadScrolling)
		.AnimateWheelScrolling(bAnimateWheelScrolling)
		.WheelScrollMultiplier(WheelScrollMultiplier)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled))
		.OnMouseScrolled(BIND_UOBJECT_DELEGATE(FOnMouseScroll, SlateHandleMouseScrolled));
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
		for (UPanelSlot* PanelSlot : Slots)
		{
			if (UPunal_ScrollBoxSlot* TypedSlot = Cast<UPunal_ScrollBoxSlot>(PanelSlot))
			{
				TypedSlot->Parent = this;
				TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
			}
		}

	return MyScrollBox.ToSharedRef();
}

void UPunal_ScrollBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyScrollBox.IsValid())
	{
		return;
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyScrollBox->SetScrollOffset(DesiredScrollOffset);
	MyScrollBox->SetOrientation(Orientation);
	MyScrollBox->SetScrollBarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(ScrollBarVisibility));
	MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	MyScrollBox->SetScrollBarPadding(ScrollbarPadding);
	MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	MyScrollBox->SetScrollBarTrackAlwaysVisible(AlwaysShowScrollbarTrack);
	MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	MyScrollBox->SetScrollBarRightClickDragAllowed(bAllowRightClickDragScrolling);
	MyScrollBox->SetConsumeMouseWheel(ConsumeMouseWheel);
	MyScrollBox->SetAnimateWheelScrolling(bAnimateWheelScrolling);
	MyScrollBox->SetWheelScrollMultiplier(WheelScrollMultiplier);
	MyScrollBox->SetStyle(&WidgetStyle);
	MyScrollBox->InvalidateStyle();
	MyScrollBox->SetScrollBarStyle(&WidgetBarStyle);
	MyScrollBox->InvalidateScrollBarStyle();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

float UPunal_ScrollBox::GetScrollOffset() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffset();
	}

	return 0;
}

float UPunal_ScrollBox::GetScrollOffsetOfEnd() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffsetOfEnd();
	}

	return 0;
}

float UPunal_ScrollBox::GetViewFraction() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetViewFraction();
	}

	return 0;
}

float UPunal_ScrollBox::GetViewOffsetFraction() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetViewOffsetFraction();
	}

	return 0;
}

void UPunal_ScrollBox::SetScrollOffset(float NewScrollOffset)
{
	DesiredScrollOffset = NewScrollOffset;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollOffset(NewScrollOffset);
	}
}

void UPunal_ScrollBox::ScrollToStart()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToStart();
	}
}

void UPunal_ScrollBox::ScrollToEnd()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToEnd();
	}
}

void UPunal_ScrollBox::ScrollWidgetIntoView(UWidget* WidgetToFind, bool AnimateScroll, EDescendantScrollDestination InScrollDestination, float Padding)
{
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MyScrollBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to scroll which is necessary to avoid warnings/ensures 
		//       when we request to scroll to a widget and later remove that widget!
		MyScrollBox->ScrollDescendantIntoView(SlateWidgetToFind, AnimateScroll, InScrollDestination, Padding);
	}
}

#if WITH_EDITORONLY_DATA

void UPunal_ScrollBox::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);

	const bool bDeprecateThickness = Ar.IsLoading() && Ar.CustomVer(FEditorObjectVersion::GUID) < FEditorObjectVersion::ScrollBarThicknessChange;
	if (bDeprecateThickness)
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			// Set ScrollbarThickness property to previous default value.
			ScrollbarThickness.Set(5.0f, 5.0f);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	Super::Serialize(Ar);

	if (bDeprecateThickness)
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			// Implicit padding of 2 was removed, so ScrollbarThickness value must be incremented by 4.
			ScrollbarThickness += FVector2D(4.0f, 4.0f);
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}
}

#endif // if WITH_EDITORONLY_DATA

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UPunal_ScrollBox::SetWidgetStyle(const FScrollBoxStyle& NewWidgetStyle)
{
	WidgetStyle = NewWidgetStyle;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->InvalidateStyle();
	}
}

const FScrollBoxStyle& UPunal_ScrollBox::GetWidgetStyle() const
{
	return WidgetStyle;
}

void UPunal_ScrollBox::SetWidgetBarStyle(const FScrollBarStyle& NewWidgetBarStyle)
{
	WidgetBarStyle = NewWidgetBarStyle;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->InvalidateScrollBarStyle();
	}
}

const FScrollBarStyle& UPunal_ScrollBox::GetWidgetBarStyle() const
{
	return WidgetBarStyle;
}

void UPunal_ScrollBox::SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination)
{
	NavigationDestination = NewNavigationDestination;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetNavigationDestination(NewNavigationDestination);
	}
}

EDescendantScrollDestination UPunal_ScrollBox::GetNavigationDestination() const
{
	return NavigationDestination;
}

void UPunal_ScrollBox::SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel)
{
	ConsumeMouseWheel = NewConsumeMouseWheel;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetConsumeMouseWheel(NewConsumeMouseWheel);
	}
}

EConsumeMouseWheel UPunal_ScrollBox::GetConsumeMouseWheel() const
{
	return ConsumeMouseWheel;
}

void UPunal_ScrollBox::SetOrientation(EOrientation NewOrientation)
{
	Orientation = NewOrientation;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetOrientation(Orientation);
	}
}

EOrientation UPunal_ScrollBox::GetOrientation() const
{
	return Orientation;
}

void UPunal_ScrollBox::SetScrollBarVisibility(ESlateVisibility NewScrollBarVisibility)
{
	ScrollBarVisibility = NewScrollBarVisibility;

	if (MyScrollBox.IsValid())
	{
		switch (ScrollBarVisibility)
		{
		case ESlateVisibility::Collapsed:				MyScrollBox->SetScrollBarVisibility(EVisibility::Collapsed); break;
		case ESlateVisibility::Hidden:					MyScrollBox->SetScrollBarVisibility(EVisibility::Hidden); break;
		case ESlateVisibility::HitTestInvisible:		MyScrollBox->SetScrollBarVisibility(EVisibility::HitTestInvisible); break;
		case ESlateVisibility::SelfHitTestInvisible:	MyScrollBox->SetScrollBarVisibility(EVisibility::SelfHitTestInvisible); break;
		case ESlateVisibility::Visible:					MyScrollBox->SetScrollBarVisibility(EVisibility::Visible); break;
		}
	}
}

ESlateVisibility UPunal_ScrollBox::GetScrollBarVisibility() const
{
	return ScrollBarVisibility;
}

void UPunal_ScrollBox::SetScrollbarThickness(const FVector2D& NewScrollbarThickness)
{
	ScrollbarThickness = NewScrollbarThickness;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	}
}

FVector2D UPunal_ScrollBox::GetScrollbarThickness() const
{
	return ScrollbarThickness;
}

void UPunal_ScrollBox::SetScrollbarPadding(const FMargin& NewScrollbarPadding)
{
	ScrollbarPadding = NewScrollbarPadding;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarPadding(ScrollbarPadding);
	}
}

FMargin UPunal_ScrollBox::GetScrollbarPadding() const
{
	return ScrollbarPadding;
}

void UPunal_ScrollBox::SetAlwaysShowScrollbar(bool NewAlwaysShowScrollbar)
{
	AlwaysShowScrollbar = NewAlwaysShowScrollbar;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	}
}

bool UPunal_ScrollBox::IsAlwaysShowScrollbar() const
{
	return AlwaysShowScrollbar;
}

void UPunal_ScrollBox::SetAllowOverscroll(bool NewAllowOverscroll)
{
	AllowOverscroll = NewAllowOverscroll;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	}
}

bool UPunal_ScrollBox::IsAllowOverscroll() const
{
	return AllowOverscroll;
}

void UPunal_ScrollBox::SetAnimateWheelScrolling(bool bShouldAnimateWheelScrolling)
{
	bAnimateWheelScrolling = bShouldAnimateWheelScrolling;
	if (MyScrollBox)
	{
		MyScrollBox->SetAnimateWheelScrolling(bShouldAnimateWheelScrolling);
	}
}

bool UPunal_ScrollBox::IsAnimateWheelScrolling() const
{
	return bAnimateWheelScrolling;
}

void UPunal_ScrollBox::SetWheelScrollMultiplier(float NewWheelScrollMultiplier)
{
	WheelScrollMultiplier = NewWheelScrollMultiplier;
	if (MyScrollBox)
	{
		MyScrollBox->SetWheelScrollMultiplier(NewWheelScrollMultiplier);
	}
}

float UPunal_ScrollBox::GetWheelScrollMultiplier() const
{
	return WheelScrollMultiplier;
}

void UPunal_ScrollBox::SetScrollWhenFocusChanges(EScrollWhenFocusChanges NewScrollWhenFocusChanges)
{
	ScrollWhenFocusChanges = NewScrollWhenFocusChanges;
	if (MyScrollBox)
	{
		MyScrollBox->SetScrollWhenFocusChanges(NewScrollWhenFocusChanges);
	}
}

EScrollWhenFocusChanges UPunal_ScrollBox::GetScrollWhenFocusChanges() const
{
	return ScrollWhenFocusChanges;
}

void UPunal_ScrollBox::EndInertialScrolling()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->EndInertialScrolling();
	}
}

void UPunal_ScrollBox::SetAlwaysShowScrollbarTrack(bool NewAlwaysShowScrollbarTrack)
{
	AlwaysShowScrollbarTrack = NewAlwaysShowScrollbarTrack;
	if (MyScrollBox)
	{
		MyScrollBox->SetScrollBarTrackAlwaysVisible(AlwaysShowScrollbarTrack);
	}
}

bool UPunal_ScrollBox::IsAlwaysShowScrollbarTrack() const
{
	return AlwaysShowScrollbarTrack;
}

float UPunal_ScrollBox::GetNavigationScrollPadding() const
{
	return NavigationScrollPadding;
}

void UPunal_ScrollBox::SetAllowRightClickDragScrolling(bool bShouldAllowRightClickDragScrolling)
{
	AlwaysShowScrollbarTrack = bShouldAllowRightClickDragScrolling;
	if (MyScrollBox)
	{
		MyScrollBox->SetScrollBarRightClickDragAllowed(AlwaysShowScrollbarTrack);
	}
}

bool UPunal_ScrollBox::IsAllowRightClickDragScrolling() const
{
	return AlwaysShowScrollbarTrack;
}

bool UPunal_ScrollBox::IsFrontPadScrolling() const
{
	return FrontPadScrolling;
}

bool UPunal_ScrollBox::IsBackPadScrolling() const
{
	return BackPadScrolling;
}

void UPunal_ScrollBox::InitBackPadScrolling(bool InBackPadScrolling)
{
	ensureMsgf(!MyScrollBox.IsValid(), TEXT("The widget is already created."));
	BackPadScrolling = InBackPadScrolling;
}

void UPunal_ScrollBox::InitFrontPadScrolling(bool InFrontPadScrolling)
{
	ensureMsgf(!MyScrollBox.IsValid(), TEXT("The widget is already created."));
	FrontPadScrolling = InFrontPadScrolling;
}

void UPunal_ScrollBox::InitNavigationScrollPadding(float InNavigationScrollPadding)
{
	ensureMsgf(!MyScrollBox.IsValid(), TEXT("The widget is already created."));
	NavigationScrollPadding = InNavigationScrollPadding;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

void UPunal_ScrollBox::SlateHandleUserScrolled(float CurrentOffset)
{
	OnUserScrolled.Broadcast(CurrentOffset);
}

void UPunal_ScrollBox::SlateHandleMouseScrolled(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OMouseScroll.Broadcast(MyGeometry, MouseEvent);
}

#if WITH_EDITOR

const FText UPunal_ScrollBox::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

void UPunal_ScrollBox::OnDescendantSelectedByDesigner(UWidget* DescendantWidget)
{
	UWidget* SelectedChild = UWidget::FindChildContainingDescendant(this, DescendantWidget);
	if (SelectedChild)
	{
		ScrollWidgetIntoView(SelectedChild, true);

		if (TickHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
			TickHandle.Reset();
		}
	}
}

void UPunal_ScrollBox::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	// because we get a deselect before we get a select, we need to delay this call until we're sure we didn't scroll to another widget.
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float) -> bool
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_UPunal_ScrollBox_ScrollToStart_LambdaTick);
			this->ScrollToStart();
			return false;
		}));
}

float UPunal_ScrollBox::GetDragTriggerDistance() const
{
	return FSlateApplication::Get().GetDragTriggerDistance();
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE