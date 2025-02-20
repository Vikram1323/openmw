#include "terrainstorage.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/esmstore.hpp"

#include "landmanager.hpp"

namespace MWRender
{

    TerrainStorage::TerrainStorage(Resource::ResourceSystem* resourceSystem, const std::string& normalMapPattern,
        const std::string& normalHeightMapPattern, bool autoUseNormalMaps, const std::string& specularMapPattern,
        bool autoUseSpecularMaps)
        : ESMTerrain::Storage(resourceSystem->getVFS(), normalMapPattern, normalHeightMapPattern, autoUseNormalMaps,
            specularMapPattern, autoUseSpecularMaps)
        , mLandManager(new LandManager(
              ESM::Land::DATA_VCLR | ESM::Land::DATA_VHGT | ESM::Land::DATA_VNML | ESM::Land::DATA_VTEX))
        , mResourceSystem(resourceSystem)
    {
        mResourceSystem->addResourceManager(mLandManager.get());
    }

    TerrainStorage::~TerrainStorage()
    {
        mResourceSystem->removeResourceManager(mLandManager.get());
    }

    bool TerrainStorage::hasData(ESM::ExteriorCellLocation cellLocation)
    {
        const MWWorld::ESMStore& esmStore = *MWBase::Environment::get().getESMStore();

        if (ESM::isEsm4Ext(cellLocation.mWorldspace))
        {
            return esmStore.get<ESM4::Land>().search(cellLocation) != nullptr;
        }
        else
        {
            return esmStore.get<ESM::Land>().search(cellLocation.mX, cellLocation.mY) != nullptr;
        }
    }

    static void BoundUnion(float& minX, float& maxX, float& minY, float& maxY, float x, float y)
    {
        if (x < minX)
            minX = x;
        if (x > maxX)
            maxX = x;
        if (y < minY)
            minY = y;
        if (y > maxY)
            maxY = y;
    }

    void TerrainStorage::getBounds(float& minX, float& maxX, float& minY, float& maxY, ESM::RefId worldspace)
    {
        minX = 0;
        minY = 0;
        maxX = 0;
        maxY = 0;

        const MWWorld::ESMStore& esmStore = *MWBase::Environment::get().getESMStore();

        if (ESM::isEsm4Ext(worldspace))
        {
            const auto& lands = esmStore.get<ESM4::Land>().getLands();
            for (const auto& [landPos, _] : lands)
            {
                if (landPos.mWorldspace == worldspace)
                {
                    BoundUnion(minX, maxX, minY, maxY, static_cast<float>(landPos.mX), static_cast<float>(landPos.mY));
                }
            }
        }
        else
        {
            MWWorld::Store<ESM::Land>::iterator it = esmStore.get<ESM::Land>().begin();
            for (; it != esmStore.get<ESM::Land>().end(); ++it)
            {
                BoundUnion(minX, maxX, minY, maxY, static_cast<float>(it->mX), static_cast<float>(it->mY));
            }
        }
        // since grid coords are at cell origin, we need to add 1 cell
        maxX += 1;
        maxY += 1;
    }

    LandManager* TerrainStorage::getLandManager() const
    {
        return mLandManager.get();
    }

    osg::ref_ptr<const ESMTerrain::LandObject> TerrainStorage::getLand(ESM::ExteriorCellLocation cellLocation)
    {
        return mLandManager->getLand(cellLocation);
    }

    const ESM::LandTexture* TerrainStorage::getLandTexture(int index, short plugin)
    {
        const MWWorld::ESMStore& esmStore = *MWBase::Environment::get().getESMStore();
        return esmStore.get<ESM::LandTexture>().search(index, plugin);
    }

}
