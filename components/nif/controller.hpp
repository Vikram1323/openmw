/*
  OpenMW - The completely unofficial reimplementation of Morrowind
  Copyright (C) 2008-2010  Nicolay Korslund
  Email: < korslund@gmail.com >
  WWW: https://openmw.org/

  This file (controller.h) is part of the OpenMW package.

  OpenMW is distributed as free software: you can redistribute it
  and/or modify it under the terms of the GNU General Public License
  version 3, as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  version 3 along with this program. If not, see
  https://www.gnu.org/licenses/ .

 */

#ifndef OPENMW_COMPONENTS_NIF_CONTROLLER_HPP
#define OPENMW_COMPONENTS_NIF_CONTROLLER_HPP

#include "base.hpp"

namespace Nif
{

    struct ControlledBlock
    {
        std::string mTargetName;
        NiInterpolatorPtr mInterpolator;
        ControllerPtr mController;
        NiBlendInterpolatorPtr mBlendInterpolator;
        unsigned short mBlendIndex;
        unsigned char mPriority;
        NiStringPalettePtr mStringPalette;
        size_t mNodeNameOffset;
        size_t mPropertyTypeOffset;
        size_t mControllerTypeOffset;
        size_t mControllerIdOffset;
        size_t mInterpolatorIdOffset;
        std::string mNodeName;
        std::string mPropertyType;
        std::string mControllerType;
        std::string mControllerId;
        std::string mInterpolatorId;

        void read(NIFStream* nif);
        void post(Reader& nif);
    };

    // Gamebryo KF root node record type (pre-10.0)
    struct NiSequence : public Record
    {
        std::string mName;
        std::string mAccumRootName;
        ExtraPtr mTextKeys;
        unsigned int mArrayGrowBy;
        std::vector<ControlledBlock> mControlledBlocks;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    // Gamebryo KF root node record type (10.0+)
    struct NiControllerSequence : public NiSequence
    {
        float mWeight{ 1.f };
        Controller::ExtrapolationMode mExtrapolationMode{ Controller::ExtrapolationMode::Constant };
        float mFrequency{ 1.f };
        float mPhase{ 1.f };
        float mStartTime, mStopTime;
        bool mPlayBackwards{ false };
        NiControllerManagerPtr mManager;
        NiStringPalettePtr mStringPalette;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    // Base class for controllers that use NiInterpolators to animate objects.
    struct NiInterpController : public Controller
    {
        // Usually one of the flags.
        bool mManagerControlled{ false };

        void read(NIFStream* nif) override;
    };

    // Base class for controllers that use one NiInterpolator.
    struct NiSingleInterpController : public NiInterpController
    {
        NiInterpolatorPtr mInterpolator;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    // Base class for controllers that use a NiFloatInterpolator to animate their target.
    struct NiFloatInterpController : public NiSingleInterpController
    {
    };

    // Ditto for NiBoolInterpolator.
    struct NiBoolInterpController : public NiSingleInterpController
    {
    };

    // Ditto for NiPoint3Interpolator.
    struct NiPoint3InterpController : public NiSingleInterpController
    {
    };

    struct NiParticleSystemController : public Controller
    {
        enum BSPArrayController
        {
            BSPArrayController_AtNode = 0x8,
            BSPArrayController_AtVertex = 0x10
        };

        struct Particle
        {
            osg::Vec3f velocity;
            float lifetime;
            float lifespan;
            float timestamp;
            unsigned short vertex;
        };

        float velocity;
        float velocityRandom;

        float verticalDir; // 0=up, pi/2=horizontal, pi=down
        float verticalAngle;
        float horizontalDir;
        float horizontalAngle;

        osg::Vec4f color;
        float size;
        float startTime;
        float stopTime;

        float emitRate;
        float lifetime;
        float lifetimeRandom;

        enum EmitFlags
        {
            EmitFlag_NoAutoAdjust = 0x1 // If this flag is set, we use the emitRate value. Otherwise,
                                        // we calculate an emit rate so that the maximum number of particles
                                        // in the system (numParticles) is never exceeded.
        };
        int emitFlags;

        osg::Vec3f offsetRandom;

        NodePtr emitter;

        int numParticles;
        int activeCount;
        std::vector<Particle> particles;

        NiParticleModifierPtr affectors;
        NiParticleModifierPtr colliders;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;

        bool noAutoAdjust() const { return emitFlags & EmitFlag_NoAutoAdjust; }
        bool emitAtVertex() const { return flags & BSPArrayController_AtVertex; }
    };
    using NiBSPArrayController = NiParticleSystemController;

    struct NiMaterialColorController : public NiPoint3InterpController
    {
        NiPosDataPtr mData;
        unsigned int mTargetColor;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiPathController : public Controller
    {
        NiPosDataPtr posData;
        NiFloatDataPtr floatData;

        enum Flags
        {
            Flag_OpenCurve = 0x020,
            Flag_AllowFlip = 0x040,
            Flag_Bank = 0x080,
            Flag_ConstVelocity = 0x100,
            Flag_Follow = 0x200,
            Flag_FlipFollowAxis = 0x400
        };

        int bankDir;
        float maxBankAngle, smoothing;
        short followAxis;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiLookAtController : public Controller
    {
        NodePtr target;
        unsigned short lookAtFlags{ 0 };

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiUVController : public Controller
    {
        NiUVDataPtr data;
        unsigned int uvSet;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiKeyframeController : public NiSingleInterpController
    {
        NiKeyframeDataPtr mData;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiMultiTargetTransformController : public NiInterpController
    {
        NodeList mExtraTargets;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiAlphaController : public NiFloatInterpController
    {
        NiFloatDataPtr mData;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiRollController : public NiSingleInterpController
    {
        NiFloatDataPtr mData;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiGeomMorpherController : public NiInterpController
    {
        bool mUpdateNormals{ false };
        bool mAlwaysActive{ false };
        NiMorphDataPtr mData;
        NiInterpolatorList mInterpolators;
        std::vector<float> mWeights;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiVisController : public NiBoolInterpController
    {
        NiVisDataPtr mData;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiFlipController : public NiFloatInterpController
    {
        int mTexSlot; // NiTexturingProperty::TextureType
        float mDelta; // Time between two flips. delta = (start_time - stop_time) / num_sources
        NiSourceTextureList mSources;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct bhkBlendController : public Controller
    {
        void read(NIFStream* nif) override;
    };

    struct NiControllerManager : public Controller
    {
        bool mCumulative;
        NiControllerSequenceList mSequences;
        NiDefaultAVObjectPalettePtr mObjectPalette;
        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiInterpolator : public Record
    {
    };

    struct NiPoint3Interpolator : public NiInterpolator
    {
        osg::Vec3f defaultVal;
        NiPosDataPtr data;
        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiBoolInterpolator : public NiInterpolator
    {
        char defaultVal;
        NiBoolDataPtr data;
        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiFloatInterpolator : public NiInterpolator
    {
        float defaultVal;
        NiFloatDataPtr data;
        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiTransformInterpolator : public NiInterpolator
    {
        osg::Vec3f defaultPos;
        osg::Quat defaultRot;
        float defaultScale;
        NiKeyframeDataPtr data;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiColorInterpolator : public NiInterpolator
    {
        osg::Vec4f defaultVal;
        NiColorDataPtr data;
        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    // Abstract
    struct NiBlendInterpolator : public NiInterpolator
    {
        struct Item
        {
            NiInterpolatorPtr mInterpolator;
            float mWeight, mNormalizedWeight;
            int mPriority;
            float mEaseSpinner;

            void read(NIFStream* nif);
            void post(Reader& nif);
        };

        bool mManagerControlled{ false };
        bool mOnlyUseHighestWeight{ false };
        unsigned short mArrayGrowBy{ 0 };
        float mWeightThreshold;
        unsigned short mInterpCount;
        unsigned short mSingleIndex;
        int mHighPriority, mNextHighPriority;
        float mSingleTime;
        float mHighWeightsSum, mNextHighWeightsSum;
        float mHighEaseSpinner;
        std::vector<Item> mItems;
        NiInterpolatorPtr mSingleInterpolator;

        void read(NIFStream* nif) override;
        void post(Reader& nif) override;
    };

    struct NiBlendBoolInterpolator : public NiBlendInterpolator
    {
        char mValue;
        void read(NIFStream* nif) override;
    };

    struct NiBlendFloatInterpolator : public NiBlendInterpolator
    {
        float mValue;
        void read(NIFStream* nif) override;
    };

    struct NiBlendPoint3Interpolator : public NiBlendInterpolator
    {
        osg::Vec3f mValue;
        void read(NIFStream* nif) override;
    };

    struct NiBlendTransformInterpolator : public NiBlendInterpolator
    {
        osg::Vec3f mPosValue;
        osg::Quat mRotValue;
        float mScaleValue;
        void read(NIFStream* nif) override;
    };

} // Namespace
#endif
