#ifndef OPENMW_MWWORLD_CELLPRELOADER_H
#define OPENMW_MWWORLD_CELLPRELOADER_H

#include <components/sceneutil/workqueue.hpp>
#include <map>
#include <osg/Vec3f>
#include <osg/Vec4i>
#include <osg/ref_ptr>

namespace Resource
{
    class ResourceSystem;
    class BulletShapeManager;
}

namespace Terrain
{
    class World;
    class View;
}

namespace MWRender
{
    class LandManager;
}

namespace Loading
{
    class Listener;
}

namespace MWWorld
{
    class CellStore;
    class TerrainPreloadItem;

    class CellPreloader
    {
    public:
        CellPreloader(Resource::ResourceSystem* resourceSystem, Resource::BulletShapeManager* bulletShapeManager,
            Terrain::World* terrain, MWRender::LandManager* landManager);
        ~CellPreloader();

        /// Ask a background thread to preload rendering meshes and collision shapes for objects in this cell.
        /// @note The cell itself must be in State_Loaded or State_Preloaded.
        void preload(MWWorld::CellStore& cell, double timestamp);

        void notifyLoaded(MWWorld::CellStore* cell);

        void clear();

        /// Removes preloaded cells that have not had a preload request for a while.
        void updateCache(double timestamp);

        /// How long to keep a preloaded cell in cache after it's no longer requested.
        void setExpiryDelay(double expiryDelay);

        /// The minimum number of preloaded cells before unused cells get thrown out.
        void setMinCacheSize(unsigned int num);

        /// The maximum number of preloaded cells.
        void setMaxCacheSize(unsigned int num);

        /// Enables the creation of instances in the preloading thread.
        void setPreloadInstances(bool preload);

        unsigned int getMaxCacheSize() const;

        void setWorkQueue(osg::ref_ptr<SceneUtil::WorkQueue> workQueue);

        typedef std::pair<osg::Vec3f, osg::Vec4i> PositionCellGrid;
        void setTerrainPreloadPositions(const std::vector<PositionCellGrid>& positions);

        void syncTerrainLoad(Loading::Listener& listener);
        void abortTerrainPreloadExcept(const PositionCellGrid* exceptPos);
        bool isTerrainLoaded(const CellPreloader::PositionCellGrid& position, double referenceTime) const;
        void setTerrain(Terrain::World* terrain);

    private:
        void clearAllTasks();

        Resource::ResourceSystem* mResourceSystem;
        Resource::BulletShapeManager* mBulletShapeManager;
        Terrain::World* mTerrain;
        MWRender::LandManager* mLandManager;
        osg::ref_ptr<SceneUtil::WorkQueue> mWorkQueue;
        double mExpiryDelay;
        unsigned int mMinCacheSize;
        unsigned int mMaxCacheSize;
        bool mPreloadInstances;

        double mLastResourceCacheUpdate;

        struct PreloadEntry
        {
            PreloadEntry(double timestamp, osg::ref_ptr<SceneUtil::WorkItem> workItem)
                : mTimeStamp(timestamp)
                , mWorkItem(workItem)
            {
            }
            PreloadEntry()
                : mTimeStamp(0.0)
            {
            }

            double mTimeStamp;
            osg::ref_ptr<SceneUtil::WorkItem> mWorkItem;
        };
        typedef std::map<const MWWorld::CellStore*, PreloadEntry> PreloadMap;

        // Cells that are currently being preloaded, or have already finished preloading
        PreloadMap mPreloadCells;

        std::vector<osg::ref_ptr<Terrain::View>> mTerrainViews;
        std::vector<PositionCellGrid> mTerrainPreloadPositions;
        osg::ref_ptr<TerrainPreloadItem> mTerrainPreloadItem;
        osg::ref_ptr<SceneUtil::WorkItem> mUpdateCacheItem;

        std::vector<PositionCellGrid> mLoadedTerrainPositions;
        double mLoadedTerrainTimestamp;
    };

}

#endif
