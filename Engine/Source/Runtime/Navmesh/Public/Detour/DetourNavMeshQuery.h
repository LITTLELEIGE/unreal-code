//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef DETOURNAVMESHQUERY_H
#define DETOURNAVMESHQUERY_H

#include "DetourNavMesh.h"
#include "DetourStatus.h"

//@UE4 BEGIN
#ifndef WITH_FIXED_AREA_ENTERING_COST
#define WITH_FIXED_AREA_ENTERING_COST 1
#endif

#ifndef FLT_MAX
#include <float.h>
#endif
#define DT_UNWALKABLE_POLY_COST FLT_MAX
//@UE4 END

// Define DT_VIRTUAL_QUERYFILTER if you wish to derive a custom filter from dtQueryFilter.
// On certain platforms indirect or virtual function call is expensive. The default
// setting is to use non-virtual functions, the actual implementations of the functions
// are declared as inline for maximum speed. 

//#define DT_VIRTUAL_QUERYFILTER 1

// Special link filter is custom filter run only for offmesh links with assigned UserId
// Used by smart navlinks in UE4
//
struct NAVMESH_API dtQuerySpecialLinkFilter
{
	virtual ~dtQuerySpecialLinkFilter() {}

	/// Returns true if special link can be visited.  (I.e. Is traversable.)
	///  @param[in]		UserId		Unique Id of link
	virtual bool isLinkAllowed(const int UserId) const { return true; }
};

/// Defines polygon filtering and traversal costs for navigation mesh query operations.
/// @ingroup detour
class NAVMESH_API dtQueryFilter
{
	float m_areaCost[DT_MAX_AREAS];		///< Cost per area type. (Used by default implementation.)
//@UE4 BEGIN
#if WITH_FIXED_AREA_ENTERING_COST
	float m_areaFixedCost[DT_MAX_AREAS];///< Fixed cost for entering an area type (Used by default implementation.)
#endif // WITH_FIXED_AREA_ENTERING_COST
	float heuristicScale;				///< Search heuristic scale.
//@UE4 END
	unsigned short m_includeFlags;		///< Flags for polygons that can be visited. (Used by default implementation.)
	unsigned short m_excludeFlags;		///< Flags for polygons that should not be visted. (Used by default implementation.)
//@UE4 BEGIN
	bool m_isBacktracking;
//@UE4 END

public:
	dtQueryFilter();
	
	/// Returns true if the polygon can be visited.  (I.e. Is traversable.)
	///  @param[in]		ref		The reference id of the polygon test.
	///  @param[in]		tile	The tile containing the polygon.
	///  @param[in]		poly  The polygon to test.
#ifdef DT_VIRTUAL_QUERYFILTER
	virtual bool passFilter(const dtPolyRef ref,
							const dtMeshTile* tile,
							const dtPoly* poly) const;
#else
//@UE4 BEGIN
	// moved here from cpp file due to the cross-lib inlining issues
	inline bool passFilter(const dtPolyRef /*ref*/,
			const dtMeshTile* /*tile*/,
			const dtPoly* poly) const
	{
		return (poly->flags & m_includeFlags) != 0 && (poly->flags & m_excludeFlags) == 0
			&& (m_areaCost[poly->getArea()] < DT_UNWALKABLE_POLY_COST) 
#if WITH_FIXED_AREA_ENTERING_COST
			&& (m_areaFixedCost[poly->getArea()] < DT_UNWALKABLE_POLY_COST)
#endif // WITH_FIXED_AREA_ENTERING_COST
			;
	}
//@UE4 END
#endif

	/// Returns cost to move from the beginning to the end of a line segment
	/// that is fully contained within a polygon.
	///  @param[in]		pa			The start position on the edge of the previous and current polygon. [(x, y, z)]
	///  @param[in]		pb			The end position on the edge of the current and next polygon. [(x, y, z)]
	///  @param[in]		prevRef		The reference id of the previous polygon. [opt]
	///  @param[in]		prevTile	The tile containing the previous polygon. [opt]
	///  @param[in]		prevPoly	The previous polygon. [opt]
	///  @param[in]		curRef		The reference id of the current polygon.
	///  @param[in]		curTile		The tile containing the current polygon.
	///  @param[in]		curPoly		The current polygon.
	///  @param[in]		nextRef		The refernece id of the next polygon. [opt]
	///  @param[in]		nextTile	The tile containing the next polygon. [opt]
	///  @param[in]		nextPoly	The next polygon. [opt]
#ifdef DT_VIRTUAL_QUERYFILTER
	virtual float getCost(const float* pa, const float* pb,
						  const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
						  const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
						  const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const;
#else
	float getCost(const float* pa, const float* pb,
				  const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly,
				  const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly,
				  const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const;
#endif

	/// @name Getters and setters for the default implementation data.
	///@{

	/// Returns the traversal cost of the area.
	///  @param[in]		i		The id of the area.
	/// @returns The traversal cost of the area.
	inline float getAreaCost(const int i) const { return m_areaCost[i]; }

	/// Sets the traversal cost of the area.
	///  @param[in]		i		The id of the area.
	///  @param[in]		cost	The new cost of traversing the area.
	inline void setAreaCost(const int i, const float cost) { m_areaCost[i] = cost; } 

//@UE4 BEGIN
	inline const float* getAllAreaCosts() const { return m_areaCost; }
	
#if WITH_FIXED_AREA_ENTERING_COST
	/// Returns the fixed cost for entering an area.
	///  @param[in]		i		The id of the area.
	///  @returns The fixed cost of the area.
	inline float getAreaFixedCost(const int i) const { return m_areaFixedCost[i]; }

	/// Sets the fixed cost for entering an area.
	///  @param[in]		i		The id of the area.
	///  @param[in]		cost	The new fixed cost of entering the area polygon.
	inline void setAreaFixedCost(const int i, const float cost) { m_areaFixedCost[i] = cost; } 

	inline const float* getAllFixedAreaCosts() const { return m_areaFixedCost; }
#endif // WITH_FIXED_AREA_ENTERING_COST

	/// Retrieves euclidean distance heuristic scale
	///  @returns heuristic scale
	inline float getHeuristicScale() const { return heuristicScale; }

	/// Retrieves euclidean distance heuristic scale
	///  @returns heuristic scale
	inline void setHeuristicScale(const float newScale) { heuristicScale = newScale; }

	/// Filters link in regards to its side. Used for backtracking.
	///  @returns should link with this side be accepted
	inline bool isValidLinkSide(const unsigned char side) const 
	{ 
		return (side & DT_LINK_FLAG_OFFMESH_CON) == 0 || (side & DT_LINK_FLAG_OFFMESH_CON_BIDIR) != 0
			|| (m_isBacktracking ? (side & DT_LINK_FLAG_OFFMESH_CON_BACKTRACKER) != 0
				: (side & DT_LINK_FLAG_OFFMESH_CON_BACKTRACKER) == 0);
	}

	/// Sets up filter for backtracking
	inline void setIsBacktracking(const bool isBacktracking) { m_isBacktracking = isBacktracking; }

	/// Retrieves information whether this filter is set up for backtracking
	///  @returns is backtracking
	inline bool getIsBacktracking() const { return m_isBacktracking; }
//@UE4 END

	/// Returns the include flags for the filter.
	/// Any polygons that include one or more of these flags will be
	/// included in the operation.
	inline unsigned short getIncludeFlags() const { return m_includeFlags; }

	/// Sets the include flags for the filter.
	/// @param[in]		flags	The new flags.
	inline void setIncludeFlags(const unsigned short flags) { m_includeFlags = flags; }

	/// Returns the exclude flags for the filter.
	/// Any polygons that include one ore more of these flags will be
	/// excluded from the operation.
	inline unsigned short getExcludeFlags() const { return m_excludeFlags; }

	/// Sets the exclude flags for the filter.
	/// @param[in]		flags		The new flags.
	inline void setExcludeFlags(const unsigned short flags) { m_excludeFlags = flags; }	

	///@}

};

/// Provides the ability to perform pathfinding related queries against
/// a navigation mesh.
/// @ingroup detour
class NAVMESH_API dtNavMeshQuery
{
public:
	dtNavMeshQuery();
	~dtNavMeshQuery();
	
	/// Initializes the query object.
	///  @param[in]		nav			Pointer to the dtNavMesh object to use for all queries.
	///  @param[in]		maxNodes	Maximum number of search nodes. [Limits: 0 < value <= 65536]
	///  @param[in]		linkFilter	Special link filter used for every query
	/// @returns The status flags for the query.
	dtStatus init(const dtNavMesh* nav, const int maxNodes, const dtQuerySpecialLinkFilter* linkFilter = 0);

	/// UE4: updates special link filter for this query
	void updateLinkFilter(const dtQuerySpecialLinkFilter* linkFilter);
	
	/// @name Standard Pathfinding Functions
	// /@{

	/// Finds a path from the start polygon to the end polygon.
	///  @param[in]		startRef	The refrence id of the start polygon.
	///  @param[in]		endRef		The reference id of the end polygon.
	///  @param[in]		startPos	A position within the start polygon. [(x, y, z)]
	///  @param[in]		endPos		A position within the end polygon. [(x, y, z)]
	///  @param[in]		filter		The polygon filter to apply to the query.
	///  @param[out]	path		An ordered list of polygon references representing the path. (Start to end.) 
	///  							[(polyRef) * @p pathCount]
	///  @param[out]	pathCount	The number of polygons returned in the @p path array.
	///  @param[in]		maxPath		The maximum number of polygons the @p path array can hold. [Limit: >= 1]
	///	 @param[out]	pathSegmentsCost	If provided will get filled will cost for every piece of path corridor
	///	 @param[out]	totalCost			If provided will get filled will total cost of path
	dtStatus findPath(dtPolyRef startRef, dtPolyRef endRef,
					  const float* startPos, const float* endPos,
					  const dtQueryFilter* filter,
					  dtPolyRef* path, int* pathCount, const int maxPath,
					  float* pathSegmentsCost, float* totalCost) const;
	
	/// Finds a path from the start polygon to the end polygon using cluster graph
	///  @param[in]		startRef			The reference id of the start polygon.
	///  @param[in]		endRef				The reference id of the end polygon.
	///  @param[in]		startPos			A position within the start polygon. [(x, y, z)]
	///  @param[in]		endPos				A position within the end polygon. [(x, y, z)]
	///  @param[in]		filter				The polygon filter to use for updating cluster links
	///  @param[out]	path				An ordered list of polygon references representing the path. (Start to end.) 
	///  									[(polyRef) * @p pathCount]
	///  @param[out]	pathCount			The number of polygons returned in the @p path array.
	///  @param[in]		maxPath				The maximum number of polygons the @p path array can hold. [Limit: >= 1]
	///	 @param[out]	pathSegmentsCost	If provided will get filled will cost for every piece of path corridor
	///  @param[out]	totalCost			If provided will get filed with total cost of path
	dtStatus findClusterPath(dtPolyRef startRef, dtPolyRef endRef,
							 const float* startPos, const float* endPos,
							 const dtQueryFilter* filter,
							 dtPolyRef* path, int* pathCount, const int maxPath,
							 float* pathSegmentsCost, float* totalCost) const; 

	/// Check if there is a path from start polygon to the end polygon using cluster graph
	/// (cheap, does not care about link costs)
	///  @param[in]		startRef			The reference id of the start polygon.
	///  @param[in]		endRef				The reference id of the end polygon.
	dtStatus testClusterPath(dtPolyRef startRef, dtPolyRef endRef) const; 

	/// Finds a path from the start polygon to the end polygon using provided cluster path
	///  @param[in]		startRef			The reference id of the start polygon.
	///  @param[in]		endRef				The reference id of the end polygon.
	///  @param[in]		startPos			A position within the start polygon. [(x, y, z)]
	///  @param[in]		endPos				A position within the end polygon. [(x, y, z)]
	///  @param[in]		filter				The polygon filter to use for updating cluster links
	///  @param[out]	clusterPath			An ordered list of cluster references representing the path. (Start to end.) 
	///  @param[out]	clusterPathSize		The number of clusters in @clusterPath
	///  @param[out]	path				An ordered list of polygon references representing the path. (Start to end.) 
	///  									[(polyRef) * @p pathCount]
	///  @param[out]	pathCount			The number of polygons returned in the @p path array.
	///  @param[in]		maxPath				The maximum number of polygons the @p path array can hold. [Limit: >= 1]
	///	 @param[out]	pathSegmentsCost	If provided will get filled will cost for every piece 
	///										of path corridor (corresponding to "path" content)
	dtStatus findPathThroughClusters(dtPolyRef startRef, dtPolyRef endRef,
									 const float* startPos, const float* endPos,
									 const dtQueryFilter* filter,
									 const dtClusterRef* clusterPath, int clusterPathSize,
									 dtPolyRef* path, int* pathCount, const int maxPath,
									 float* pathSegmentsCost) const; 

	/// Finds a cluster path from the start polygon to the end polygon, DOES NOT operate on polygon level
	///  @param[in]		startRef		The reference id of the start polygon.
	///  @param[in]		endRef			The reference id of the end polygon.
	///  @param[in]		filter			The polygon filter to use for updating cluster links
	///  @param[out]	clusterPath		An ordered list of cluster references representing the path. (Start to end.) 
	///  @param[out]	clusterPathSize	The number of polygons returned in the @p clusterPath array.
	///  @param[in]		maxPath			The maximum number of clusters the @p clusterPath array can hold. [Limit: >= 1]
	///	 @param[out]	totalCost		(Optional) Total cost of path.
	dtStatus findPathOnClusterGraph(dtPolyRef startRef, dtPolyRef endRef, const dtQueryFilter* filter,
									dtClusterRef* clusterPath, int* clusterPathSize, int maxPath,
									float* totalCost) const;

	/// Finds the straight path from the start to the end position within the polygon corridor.
	///  @param[in]		startPos			Path start position. [(x, y, z)]
	///  @param[in]		endPos				Path end position. [(x, y, z)]
	///  @param[in]		path				An array of polygon references that represent the path corridor.
	///  @param[in]		pathSize			The number of polygons in the @p path array.
	///  @param[out]	straightPath		Points describing the straight path. [(x, y, z) * @p straightPathCount].
	///  @param[out]	straightPathFlags	Flags describing each point. (See: #dtStraightPathFlags) [opt]
	///  @param[out]	straightPathRefs	The reference id of the polygon that is being entered at each point. [opt]
	///  @param[out]	straightPathCount	The number of points in the straight path.
	///  @param[in]		maxStraightPath		The maximum number of points the straight path arrays can hold.  [Limit: > 0]
	///  @param[in]		options				Query options. (see: #dtStraightPathOptions)
	/// @returns The status flags for the query.
	dtStatus findStraightPath(const float* startPos, const float* endPos,
							  const dtPolyRef* path, const int pathSize,
							  float* straightPath, unsigned char* straightPathFlags, dtPolyRef* straightPathRefs,
							  int* straightPathCount, const int maxStraightPath, const int options = 0) const;

	///@}
	/// @name Sliced Pathfinding Functions
	/// Common use case:
	///	-# Call initSlicedFindPath() to initialize the sliced path query.
	///	-# Call updateSlicedFindPath() until it returns complete.
	///	-# Call finalizeSlicedFindPath() to get the path.
	///@{ 

	/// Intializes a sliced path query.
	///  @param[in]		startRef	The refrence id of the start polygon.
	///  @param[in]		endRef		The reference id of the end polygon.
	///  @param[in]		startPos	A position within the start polygon. [(x, y, z)]
	///  @param[in]		endPos		A position within the end polygon. [(x, y, z)]
	///  @param[in]		filter		The polygon filter to apply to the query.
	/// @returns The status flags for the query.
	dtStatus initSlicedFindPath(dtPolyRef startRef, dtPolyRef endRef,
								const float* startPos, const float* endPos,
								const dtQueryFilter* filter);

	/// Updates an in-progress sliced path query.
	///  @param[in]		maxIter		The maximum number of iterations to perform.
	///  @param[out]	doneIters	The actual number of iterations completed. [opt]
	/// @returns The status flags for the query.
	dtStatus updateSlicedFindPath(const int maxIter, int* doneIters);

	/// Finalizes and returns the results of a sliced path query.
	///  @param[out]	path		An ordered list of polygon references representing the path. (Start to end.) 
	///  							[(polyRef) * @p pathCount]
	///  @param[out]	pathCount	The number of polygons returned in the @p path array.
	///  @param[in]		maxPath		The max number of polygons the path array can hold. [Limit: >= 1]
	/// @returns The status flags for the query.
	dtStatus finalizeSlicedFindPath(dtPolyRef* path, int* pathCount, const int maxPath);
	
	/// Finalizes and returns the results of an incomplete sliced path query, returning the path to the furthest
	/// polygon on the existing path that was visited during the search.
	///  @param[out]	existing		An array of polygon references for the existing path.
	///  @param[out]	existingSize	The number of polygon in the @p existing array.
	///  @param[out]	path			An ordered list of polygon references representing the path. (Start to end.) 
	///  								[(polyRef) * @p pathCount]
	///  @param[out]	pathCount		The number of polygons returned in the @p path array.
	///  @param[in]		maxPath			The max number of polygons the @p path array can hold. [Limit: >= 1]
	/// @returns The status flags for the query.
	dtStatus finalizeSlicedFindPathPartial(const dtPolyRef* existing, const int existingSize,
										   dtPolyRef* path, int* pathCount, const int maxPath);

	///@}
	/// @name Dijkstra Search Functions
	/// @{ 

	/// Finds the polygons along the navigation graph that touch the specified circle.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		centerPos		The center of the search circle. [(x, y, z)]
	///  @param[in]		radius			The radius of the search circle.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[out]	resultRef		The reference ids of the polygons touched by the circle. [opt]
	///  @param[out]	resultParent	The reference ids of the parent polygons for each result. 
	///  								Zero if a result polygon has no parent. [opt]
	///  @param[out]	resultCost		The search cost from @p centerPos to the polygon. [opt]
	///  @param[out]	resultCount		The number of polygons found. [opt]
	///  @param[in]		maxResult		The maximum number of polygons the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus findPolysAroundCircle(dtPolyRef startRef, const float* centerPos, const float radius,
								   const dtQueryFilter* filter,
								   dtPolyRef* resultRef, dtPolyRef* resultParent, float* resultCost,
								   int* resultCount, const int maxResult) const;
	
	/// Finds the polygons along the naviation graph that touch the specified convex polygon.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		verts			The vertices describing the convex polygon. (CCW) 
	///  								[(x, y, z) * @p nverts]
	///  @param[in]		nverts			The number of vertices in the polygon.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[out]	resultRef		The reference ids of the polygons touched by the search polygon. [opt]
	///  @param[out]	resultParent	The reference ids of the parent polygons for each result. Zero if a 
	///  								result polygon has no parent. [opt]
	///  @param[out]	resultCost		The search cost from the centroid point to the polygon. [opt]
	///  @param[out]	resultCount		The number of polygons found.
	///  @param[in]		maxResult		The maximum number of polygons the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus findPolysAroundShape(dtPolyRef startRef, const float* verts, const int nverts,
								  const dtQueryFilter* filter,
								  dtPolyRef* resultRef, dtPolyRef* resultParent, float* resultCost,
								  int* resultCount, const int maxResult) const;
	
	//@UE4 BEGIN
	/// Finds the polygons along the navigation graph that are no more than given path length away from centerPos.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		centerPos		The center of the search circle. [(x, y, z)]
	///  @param[in]		pathDistance	The path distance limit of the search
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[out]	resultRef		The reference ids of the polygons touched by the circle. [opt]
	///  @param[out]	resultCount		The number of polygons found. [opt]
	///  @param[in]		maxResult		The maximum number of polygons the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus findPolysInPathDistance(dtPolyRef startRef, const float* centerPos, const float pathDistance,
									const dtQueryFilter* filter, dtPolyRef* resultRef,
									int* resultCount, const int maxResult) const;

	/// Finds the clusters along the navigation graph that are no more than given path length away from centerPos.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		centerPos		The center of the search circle. [(x, y, z)]
	///  @param[in]		pathDistance	The path distance limit of the search
	///  @param[in]		filter			The polygon filter to use for updating cluster links
	///  @param[out]	resultRef		The reference ids of the clusters touched by the circle. [opt]
	///  @param[out]	resultCount		The number of clusters found. [opt]
	///  @param[in]		maxResult		The maximum number of clusters the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus findClustersInPathDistance(dtPolyRef startRef, const float* centerPos,
										const float pathDistance, const dtQueryFilter* filter,
										dtClusterRef* resultRef, int* resultCount, const int maxResult) const;

	//@UE4 END

	/// @}
	/// @name Local Query Functions
	///@{

	/// Finds the polygon nearest to the specified center point.
	///  @param[in]		center		The center of the search box. [(x, y, z)]
	///  @param[in]		extents		The search distance along each axis. [(x, y, z)]
	///  @param[in]		filter		The polygon filter to apply to the query.
	///  @param[out]	nearestRef	The reference id of the nearest polygon.
	///  @param[out]	nearestPt	The nearest point on the polygon. [opt] [(x, y, z)]
	/// @returns The status flags for the query.
	dtStatus findNearestPoly(const float* center, const float* extents,
							 const dtQueryFilter* filter,
							 dtPolyRef* nearestRef, float* nearestPt) const;
	
	/// Finds polygons that overlap the search box.
	///  @param[in]		center		The center of the search box. [(x, y, z)]
	///  @param[in]		extents		The search distance along each axis. [(x, y, z)]
	///  @param[in]		filter		The polygon filter to apply to the query.
	///  @param[out]	polys		The reference ids of the polygons that overlap the query box.
	///  @param[out]	polyCount	The number of polygons in the search result.
	///  @param[in]		maxPolys	The maximum number of polygons the search result can hold.
	/// @returns The status flags for the query.
	dtStatus queryPolygons(const float* center, const float* extents,
						   const dtQueryFilter* filter,
						   dtPolyRef* polys, int* polyCount, const int maxPolys) const;

	/// Finds the non-overlapping navigation polygons in the local neighbourhood around the center position.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		centerPos		The center of the query circle. [(x, y, z)]
	///  @param[in]		radius			The radius of the query circle.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///	 @param[in]		forceIncludePath		UE4: force adding polys excluded by filter if they are part of path
	///	 @param[in]		forceIncludePathCount	UE4: the number of polys in forceIncludePath
	///  @param[out]	resultRef		The reference ids of the polygons touched by the circle.
	///  @param[out]	resultParent	The reference ids of the parent polygons for each result. 
	///  								Zero if a result polygon has no parent. [opt]
	///  @param[out]	resultCount		The number of polygons found.
	///  @param[in]		maxResult		The maximum number of polygons the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus findLocalNeighbourhood(dtPolyRef startRef, const float* centerPos, const float radius,
									const dtQueryFilter* filter,
									const dtPolyRef* forceIncludePath, const int forceIncludePathCount,
									dtPolyRef* resultRef, dtPolyRef* resultParent,
									int* resultCount, const int maxResult) const;

	/// Moves from the start to the end position constrained to the navigation mesh.
	///  @param[in]		startRef		The reference id of the start polygon.
	///  @param[in]		startPos		A position of the mover within the start polygon. [(x, y, x)]
	///  @param[in]		endPos			The desired end position of the mover. [(x, y, z)]
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[out]	resultPos		The result position of the mover. [(x, y, z)]
	///  @param[out]	visited			The reference ids of the polygons visited during the move.
	///  @param[out]	visitedCount	The number of polygons visited during the move.
	///  @param[in]		maxVisitedSize	The maximum number of polygons the @p visited array can hold.
	/// @returns The status flags for the query.
	dtStatus moveAlongSurface(dtPolyRef startRef, const float* startPos, const float* endPos,
							  const dtQueryFilter* filter,
							  float* resultPos, dtPolyRef* visited, int* visitedCount, const int maxVisitedSize) const;
	
	/// Casts a 'walkability' ray along the surface of the navigation mesh from 
	/// the start position toward the end position.
	///  @param[in]		startRef	The reference id of the start polygon.
	///  @param[in]		startPos	A position within the start polygon representing 
	///  							the start of the ray. [(x, y, z)]
	///  @param[in]		endPos		The position to cast the ray toward. [(x, y, z)]
	///  @param[out]	t			The hit parameter. (FLT_MAX if no wall hit.)
	///  @param[out]	hitNormal	The normal of the nearest wall hit. [(x, y, z)]
	///  @param[in]		filter		The polygon filter to apply to the query.
	///  @param[out]	path		The reference ids of the visited polygons. [opt]
	///  @param[out]	pathCount	The number of visited polygons. [opt]
	///  @param[in]		maxPath		The maximum number of polygons the @p path array can hold.
	///  @param[in]		walkableArea Specific area that can be visited or 255 to skip it and test all areas.
	/// @returns The status flags for the query.
	dtStatus raycast(dtPolyRef startRef, const float* startPos, const float* endPos,
					 const dtQueryFilter* filter,
					 float* t, float* hitNormal, dtPolyRef* path, int* pathCount, const int maxPath) const;
	
	/// Finds the distance from the specified position to the nearest polygon wall.
	///  @param[in]		startRef		The reference id of the polygon containing @p centerPos.
	///  @param[in]		centerPos		The center of the search circle. [(x, y, z)]
	///  @param[in]		maxRadius		The radius of the search circle.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[out]	hitDist			The distance to the nearest wall from @p centerPos.
	///  @param[out]	hitPos			The nearest position on the wall that was hit. [(x, y, z)]
	///  @param[out]	hitNormal		The normalized ray formed from the wall point to the 
	///  								source point. [(x, y, z)]
	/// @returns The status flags for the query.
	dtStatus findDistanceToWall(dtPolyRef startRef, const float* centerPos, const float maxRadius,
								const dtQueryFilter* filter,
								float* hitDist, float* hitPos, float* hitNormal) const;
	
	/// Returns the segments for the specified polygon, optionally including portals.
	///  @param[in]		ref				The reference id of the polygon.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///	 @param[in]		forceIncludePath		UE4: force accepting polys excluded by filter if they are part of path
	///	 @param[in]		forceIncludePathCount	UE4: the number of polys in forceIncludePath
	///  @param[out]	segmentVerts	The segments. [(ax, ay, az, bx, by, bz) * segmentCount]
	///  @param[out]	segmentRefs		The reference ids of each segment's neighbor polygon. 
	///  								Or zero if the segment is a wall. [opt] [(parentRef) * @p segmentCount] 
	///  @param[out]	segmentCount	The number of segments returned.
	///  @param[in]		maxSegments		The maximum number of segments the result arrays can hold.
	/// @returns The status flags for the query.
	dtStatus getPolyWallSegments(dtPolyRef ref, const dtQueryFilter* filter,
								 const dtPolyRef* forceIncludePath, const int forceIncludePathCount,
								 float* segmentVerts, dtPolyRef* segmentRefs, int* segmentCount,
								 const int maxSegments) const;

	/// Returns random location on navmesh.
	/// Polygons are chosen weighted by area. The search runs in linear related to number of polygon.
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[in]		frand			Function returning a random number [0..1).
	///  @param[out]	randomRef		The reference id of the random location.
	///  @param[out]	randomPt		The random location. 
	/// @returns The status flags for the query.
	dtStatus findRandomPoint(const dtQueryFilter* filter, float (*frand)(),
							 dtPolyRef* randomRef, float* randomPt) const;

	/// Returns random location on navmesh within the reach of specified location.
	/// Polygons are chosen weighted by area. The search runs in linear related to number of polygon.
	/// The location is not exactly constrained by the circle, but it limits the visited polygons.
	///  @param[in]		startRef		The reference id of the polygon where the search starts.
	///  @param[in]		centerPos		The center of the search circle. [(x, y, z)]
	///  @param[in]		filter			The polygon filter to apply to the query.
	///  @param[in]		frand			Function returning a random number [0..1).
	///  @param[out]	randomRef		The reference id of the random location.
	///  @param[out]	randomPt		The random location. [(x, y, z)]
	/// @returns The status flags for the query.
	dtStatus findRandomPointAroundCircle(dtPolyRef startRef, const float* centerPos, const float maxRadius,
										 const dtQueryFilter* filter, float (*frand)(),
										 dtPolyRef* randomRef, float* randomPt) const;

	/// Returns random location on navmesh within specified cluster.
	///  @param[in]		frand			Function returning a random number [0..1).
	///  @param[out]	randomRef		The reference id of the random location.
	///  @param[out]	randomPt		The random location. 
	/// @returns The status flags for the query.
	dtStatus findRandomPointInCluster(dtClusterRef clusterRef, float (*frand)(),
									  dtPolyRef* randomRef, float* randomPt) const;
	
	/// Finds the closest point on the specified polygon.
	///  @param[in]		ref			The reference id of the polygon.
	///  @param[in]		pos			The position to check. [(x, y, z)]
	///  @param[out]	closest		The closest point on the polygon. [(x, y, z)]
	/// @returns The status flags for the query.
	dtStatus closestPointOnPoly(dtPolyRef ref, const float* pos, float* closest) const;
	
	/// Returns a point on the boundary closest to the source point if the source point is outside the 
	/// polygon's xz-bounds.
	///  @param[in]		ref			The reference id to the polygon.
	///  @param[in]		pos			The position to check. [(x, y, z)]
	///  @param[out]	closest		The closest point. [(x, y, z)]
	/// @returns The status flags for the query.
	dtStatus closestPointOnPolyBoundary(dtPolyRef ref, const float* pos, float* closest) const;
	
	/// Gets the height of the polygon at the provided position using the height detail. (Most accurate.)
	///  @param[in]		ref			The reference id of the polygon.
	///  @param[in]		pos			A position within the xz-bounds of the polygon. [(x, y, z)]
	///  @param[out]	height		The height at the surface of the polygon.
	/// @returns The status flags for the query.
	dtStatus getPolyHeight(dtPolyRef ref, const float* pos, float* height) const;

	/// Gets the cluster containing given polygon
	///  @param[in]		polyRef		The reference id of the polygon.
	///  @param[out]	clusterRef	The reference id of the cluster
	/// @returns The status flags for the query.
	dtStatus getPolyCluster(dtPolyRef polyRef, dtClusterRef& clusterRef) const;

	/// @}
	/// @name Miscellaneous Functions
	/// @{

	/// Returns true if the polygon reference is valid and passes the filter restrictions.
	///  @param[in]		ref			The polygon reference to check.
	///  @param[in]		filter		The filter to apply.
	bool isValidPolyRef(dtPolyRef ref, const dtQueryFilter* filter) const;

	/// Returns true if the polygon reference is in the closed list. 
	///  @param[in]		ref		The reference id of the polygon to check.
	/// @returns True if the polygon is in closed list.
	bool isInClosedList(dtPolyRef ref) const;
	
	/// Returns true if the cluster link was used in previous search. 
	///  @param[in]		cFrom		The reference id of the start cluster.
	///  @param[in]		cto			The reference id of the end cluster.
	/// @returns True if the cluster link was searched.
	bool wasClusterLinkSearched(dtPolyRef cFrom, dtPolyRef cTo) const;

	/// Gets the node pool.
	/// @returns The node pool.
	class dtNodePool* getNodePool() const { return m_nodePool; }
	
	/// Gets the navigation mesh the query object is using.
	/// @return The navigation mesh the query object is using.
	const dtNavMesh* getAttachedNavMesh() const { return m_nav; }

	/// Gets best node ref and cost from sliced pathfinding data
	void getCurrentBestResult(struct dtNode*& bestNode, float& bestCost) const { bestNode = m_query.lastBestNode; bestCost = m_query.lastBestNodeCost; }

	float getQueryTime() const { return m_queryTime; }
	float getClusterTime() const { return m_clusterTime; }
	float getExpandTime() const { return m_expandTime; }
	int getQueryNodes() const { return m_queryNodes; }
	/// @}
	
private:
	
	/// Returns neighbour tile based on side.
	dtMeshTile* getNeighbourTileAt(int x, int y, int side) const;

	/// Queries polygons within a tile.
	int queryPolygonsInTile(const dtMeshTile* tile, const float* qmin, const float* qmax, const dtQueryFilter* filter,
							dtPolyRef* polys, const int maxPolys) const;
	/// Find nearest polygon within a tile.
	dtPolyRef findNearestPolyInTile(const dtMeshTile* tile, const float* center, const float* extents,
									const dtQueryFilter* filter, float* nearestPt) const;
	/// Returns closest point on polygon.
	void closestPointOnPolyInTile(const dtMeshTile* tile, const dtPoly* poly, const float* pos, float* closest) const;
	
	//@UE4 BEGIN
	// exposing function to be able to generate navigation corridors as sequence of point pairs
public:
	/// Returns portal points between two polygons.
	dtStatus getPortalPoints(dtPolyRef from, dtPolyRef to, float* left, float* right,
							 unsigned char& fromType, unsigned char& toType) const;
private:
	//@UE4 END
	dtStatus getPortalPoints(dtPolyRef from, const dtPoly* fromPoly, const dtMeshTile* fromTile,
							 dtPolyRef to, const dtPoly* toPoly, const dtMeshTile* toTile,
							 float* left, float* right) const;
	
	/// Returns edge mid point between two polygons.
	dtStatus getEdgeMidPoint(dtPolyRef from, dtPolyRef to, float* mid) const;
	dtStatus getEdgeMidPoint(dtPolyRef from, const dtPoly* fromPoly, const dtMeshTile* fromTile,
							 dtPolyRef to, const dtPoly* toPoly, const dtMeshTile* toTile,
							 float* mid) const;
	
	// Appends vertex to a straight path
	dtStatus appendVertex(const float* pos, const unsigned char flags, const dtPolyRef ref,
						  float* straightPath, unsigned char* straightPathFlags, dtPolyRef* straightPathRefs,
						  int* straightPathCount, const int maxStraightPath) const;

	// Appends intermediate portal points to a straight path.
	dtStatus appendPortals(const int startIdx, const int endIdx, const float* endPos, const dtPolyRef* path,
						   float* straightPath, unsigned char* straightPathFlags, dtPolyRef* straightPathRefs,
						   int* straightPathCount, const int maxStraightPath, const int options) const;
	
	inline bool passLinkFilterByRef(const dtMeshTile* tile, const dtPolyRef ref) const
	{
		return passLinkFilter(tile, m_nav->decodePolyIdPoly(ref));
	}

	inline bool passLinkFilter(const dtMeshTile* tile, const int polyIdx) const
	{
		// check if poly is offmesh link with assigned UserId
		if (m_linkFilter && polyIdx >= tile->header->offMeshBase)
		{
			const int linkIdx = polyIdx - tile->header->offMeshBase;
			if (linkIdx < tile->header->offMeshConCount &&
				tile->offMeshCons[linkIdx].userId != 0 &&
				m_linkFilter->isLinkAllowed(tile->offMeshCons[linkIdx].userId) == false)
			{
				return false;
			}
		}

		return true;
	}

	const dtNavMesh* m_nav;							///< Pointer to navmesh data.
	const dtQuerySpecialLinkFilter* m_linkFilter;	///< Pointer to optional special link filter

	struct dtQueryData
	{
		dtStatus status;
		struct dtNode* lastBestNode;
		float lastBestNodeCost;
		dtPolyRef startRef, endRef;
		float startPos[3], endPos[3];
		const dtQueryFilter* filter;
	};
	dtQueryData m_query;				///< Sliced query state.

	class dtNodePool* m_tinyNodePool;	///< Pointer to small node pool.
	class dtNodePool* m_nodePool;		///< Pointer to node pool.
	class dtNodeQueue* m_openList;		///< Pointer to open list queue.

	mutable float m_queryTime;
	mutable float m_clusterTime;
	mutable float m_expandTime;
	mutable int m_queryNodes;
};

/// Allocates a query object using the Detour allocator.
/// @return An allocated query object, or null on failure.
/// @ingroup detour
NAVMESH_API dtNavMeshQuery* dtAllocNavMeshQuery();

/// Frees the specified query object using the Detour allocator.
///  @param[in]		query		A query object allocated using #dtAllocNavMeshQuery
/// @ingroup detour
NAVMESH_API void dtFreeNavMeshQuery(dtNavMeshQuery* query);

#endif // DETOURNAVMESHQUERY_H
