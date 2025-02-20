#ifndef OPENMW_MWRENDER_PRECIPITATIONOCCLUSION_H
#define OPENMW_MWRENDER_PRECIPITATIONOCCLUSION_H

#include <osg/Camera>
#include <osg/Texture2D>

namespace MWRender
{
    class PrecipitationOccluder
    {
    public:
        PrecipitationOccluder(osg::Group* skyNode, osg::Group* sceneNode, osg::Group* rootNode, osg::Camera* camera);

        void update();

        void enable();

        void disable();

        void updateRange(const osg::Vec3f range);

    private:
        osg::Group* mSkyNode;
        osg::Group* mSceneNode;
        osg::Group* mRootNode;
        osg::ref_ptr<osg::Camera> mCamera;
        osg::ref_ptr<osg::Camera> mSceneCamera;
        osg::ref_ptr<osg::Texture2D> mDepthTexture;
        osg::Vec3f mRange;
    };
}

#endif
