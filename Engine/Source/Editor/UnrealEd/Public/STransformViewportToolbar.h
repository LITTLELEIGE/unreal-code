// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#ifndef __STransformViewportToolBar_h__
#define __STransformViewportToolBar_h__

#include "SViewportToolBar.h"

/**
 * Viewport toolbar containing transform, grid snapping, local to world and camera speed controls.
 */
class UNREALED_API STransformViewportToolBar  : public SViewportToolBar
{

public:
	SLATE_BEGIN_ARGS( STransformViewportToolBar ){}
		SLATE_ARGUMENT( TSharedPtr<class SEditorViewport>, Viewport )
		SLATE_ARGUMENT( TSharedPtr<FUICommandList>, CommandList )
		SLATE_ARGUMENT( TSharedPtr<FExtender>, Extenders )
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );
	/**
	 * Static: Creates a widget for the main tool bar
	 *
	 * @return	New widget
	 */
	TSharedRef< SWidget > MakeTransformToolBar( const TSharedPtr< FExtender > InExtenders );

private:
	FSlateIcon GetLocalToWorldIcon() const;

	/** Camera speed menu construction callback */
	TSharedRef<SWidget> FillCameraSpeedMenu();

	/** Camera speed Label callback */
	FText GetCameraSpeedLabel() const;

	/** Returns the current camera speed setting */
	float GetCamSpeedSliderPosition() const;

	/** 
	 * Sets new camera speed 
	 *
	 * @Param	NewValue	Value to set camera speed too
	 */
	void OnSetCamSpeed(float NewValue);

	/** Grid snap label callbacks */
	FText GetLocationGridLabel() const;
	FText GetRotationGridLabel() const;
	FText GetScaleGridLabel() const;

	/** GridSnap menu construction callbacks */
	TSharedRef<SWidget> FillLocationGridSnapMenu();
	TSharedRef<SWidget> FillRotationGridSnapMenu();
	TSharedRef<SWidget> FillScaleGridSnapMenu();

	/** Grid snap setting callbacks */
	static void SetGridSize( int32 InIndex );
	static void SetRotationGridSize( int32 InIndex, ERotationGridMode InGridMode );
	static void SetScaleGridSize( int32 InIndex );

	/** Grid snap is checked callbacks for the menu values */
	static bool IsGridSizeChecked( int32 GridSizeIndex );
	static bool IsRotationGridSizeChecked( int32 GridSizeIndex, ERotationGridMode GridMode );
	static bool IsScaleGridSizeChecked( int32 GridSizeIndex );

	/** Callbacks for preserving non-uniform scaling when snapping */
	static void TogglePreserveNonUniformScale();
	static bool IsPreserveNonUniformScaleChecked();
	
	/** Methods to build more complex duel lists */
	TSharedRef<SWidget> BuildLocationGridCheckBoxList(FName InExtentionHook, const FText& InHeading, const TArray<float>& InGridSizes) const;
	TSharedRef<SWidget> BuildRotationGridCheckBoxList(FName InExtentionHook, const FText& InHeading, const TArray<float>& InGridSizes, ERotationGridMode InGridMode) const;

	/** Make the surface snapping toolbar checkbox button */
	TSharedRef< SWidget > MakeSurfaceSnappingButton( FName ToolBarStyle );

	/** Grid Snap checked state callbacks */
	ESlateCheckBoxState::Type IsLocationGridSnapChecked() const;
	ESlateCheckBoxState::Type IsRotationGridSnapChecked() const;
	ESlateCheckBoxState::Type IsScaleGridSnapChecked() const;

	/** Grid snap toggle handlers */
	void HandleToggleLocationGridSnap(ESlateCheckBoxState::Type InState);
	void HandleToggleRotationGridSnap(ESlateCheckBoxState::Type InState);
	void HandleToggleScaleGridSnap(ESlateCheckBoxState::Type InState);

private:

	/** Reference to the camera slider used to display current camera speed */
	mutable TSharedPtr< SSlider > CamSpeedSlider;

	/** The editor viewport that we are in */
	TWeakPtr<class SEditorViewport> Viewport;

	/** Command list */
	TSharedPtr<FUICommandList> CommandList;
};

#endif //__STransformViewportToolBar_h__

