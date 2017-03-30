// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditableMeshTypes.h"
#include "MeshElement.h"


class IMeshEditorModeEditingContract
{

public:

	/** Gets an editable mesh from our cache of editable meshes for the specified sub-mesh address */
	virtual const class UEditableMesh* FindEditableMesh( class UPrimitiveComponent& Component, const FEditableMeshSubMeshAddress& SubMeshAddress ) const = 0;

	/** Gets the interactive action currently being performed (and previewed).  These usually happen over multiple frames, and
	    result in a 'final' application of the change that performs a more exhaustive (and more expensive) update. */
	virtual FName GetActiveAction() const = 0;

	/** Stores undo state for the specified object.  This will store the state different depending on whether we're
	    currently in the middle of previewing a temporary change to meshes (bIsCapturingUndoForPreview) */
	virtual void TrackUndo( UObject* Object, TUniquePtr<FChange> RevertChange ) = 0;

	/** Returns true if the specified element is selected right now */
	virtual bool IsMeshElementSelected( const FMeshElement MeshElement ) const = 0;

	/** Returns the mesh element the specified viewport interactor is currently hovering over.  The returned element might be invalid if nothing valid is hovered right now */
	virtual FMeshElement GetHoveredMeshElement( class UViewportInteractor* ViewportInteractor ) const = 0;

	virtual void GetSelectedMeshesAndVertices( TMap<class UEditableMesh*, TArray<FMeshElement>>& OutMeshesAndVertices ) = 0;
	virtual void GetSelectedMeshesAndEdges( TMap<class UEditableMesh*, TArray<FMeshElement>>& OutMeshesAndEdges ) = 0;
	virtual void GetSelectedMeshesAndPolygons( TMap<class UEditableMesh*, TArray<FMeshElement>>& OutMeshesAndPolygons ) = 0;

	/** Selects the specified mesh elements */
	virtual void SelectMeshElements( const TArray<FMeshElement>& MeshElementsToSelect ) = 0;

	/** Deselects all mesh elements */
	virtual void DeselectAllMeshElements() = 0;

	/** Deselects the specified mesh elements */
	virtual void DeselectMeshElements( const TArray<FMeshElement>& MeshElementsToDeselect ) = 0;
	virtual void DeselectMeshElements( const TMap<UEditableMesh*, TArray<FMeshElement>>& MeshElementsToDeselect ) = 0;

	/** Commits all selected meshes */
	virtual void CommitSelectedMeshes() = 0;

	/** Clears hover and selection on mesh elements that may no longer be valid.  You'll want to call this if you change the mesh topology */
	virtual void ClearInvalidSelectedElements() = 0;

	/** Given an interactor and a mesh, finds edges under the interactor along with their exact split position (progress along the edge) */
	virtual void FindEdgeSplitUnderInteractor( class UViewportInteractor* ViewportInteractor, const UEditableMesh* EditableMesh, const TArray<FMeshElement>& EdgeElements, TArray<float>& OutSplits ) = 0;

	/** When performing an interactive action that was initiated using an interactor, this is the interactor that was used. */
	virtual class UViewportInteractor* GetActiveActionInteractor() = 0;

};
