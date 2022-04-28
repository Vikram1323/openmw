#ifndef OPENMW_MWRENDER_PINGPONGCULL_H
#define OPENMW_MWRENDER_PINGPONGCULL_H

#include <array>

#include <components/sceneutil/nodecallback.hpp>

#include "postprocessor.hpp"

namespace MWRender
{
    class PostProcessor;
    class PingPongCull : public SceneUtil::NodeCallback<PingPongCull, osg::Node*, osgUtil::CullVisitor*>
    {
    public:
        PingPongCull();

        void operator()(osg::Node* node, osgUtil::CullVisitor* nv);

    private:
        unsigned int mLastFrameNumber;
        osg::Matrix mLastViewMatrix;
        float mLastSimulationTime;
    };
}

#endif
