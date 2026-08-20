/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gfx_utils/diagram/common/paint.h"

#include <gtest/gtest.h>

#if defined(FEATURE_COMPONENT_SVG) && FEATURE_COMPONENT_SVG
#include "gfx_utils/diagram/rasterizer/rasterizer_scanline_antialias.h"
#endif

using namespace testing::ext;
namespace OHOS {
class PaintSvgTest : public testing::Test {
};

#if defined(FEATURE_COMPONENT_SVG) && FEATURE_COMPONENT_SVG
/**
 * @tc.name: PaintSvgTest_InitRenderState_001
 * @tc.desc: Verify InitRenderState copies gradient, pattern, composite, transform and filling rule.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_InitRenderState_001, TestSize.Level1)
{
    Paint source;
    source.createLinearGradient(0.0f, 0.0f, 100.0f, 100.0f);
    source.SetLinearGradientScale(2.0f, 3.0f);
    source.addColorStop(0.0f, Color::Red());
    source.addColorStop(1.0f, Color::Blue());

    source.createRadialGradient(0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 25.0f);
    source.SetRadialGradientScale(4.0f, 5.0f);

    source.CreatePattern("test", REPEAT_X);

    source.SetShadowBlur(10);
    source.SetShadowOffsetX(1.0f);
    source.SetShadowOffsetY(2.0f);
    source.SetShadowColor(Color::Gray());

    source.SetGlobalAlpha(0.5f);
    source.SetGlobalCompositeOperation(LIGHTER);
    source.SetTransform(1.0f, 0.0f, 0.0f, 1.0f, 10, 20);
    source.Rotate(30.0f);
    source.SetFillingRule(FILL_EVEN_ODD);

    Paint dest;
    dest.SetFillingRule(FILL_NON_ZERO);
    dest.InitRenderState(source);

    EXPECT_EQ(dest.GetLinearGradientPoint().x0, source.GetLinearGradientPoint().x0);
    EXPECT_EQ(dest.GetLinearGradientPoint().y0, source.GetLinearGradientPoint().y0);
    EXPECT_EQ(dest.GetLinearGradientPoint().x1, source.GetLinearGradientPoint().x1);
    EXPECT_EQ(dest.GetLinearGradientPoint().y1, source.GetLinearGradientPoint().y1);
    EXPECT_FLOAT_EQ(dest.GetLinearGradientScaleX(), 2.0f);
    EXPECT_FLOAT_EQ(dest.GetLinearGradientScaleY(), 3.0f);
    EXPECT_EQ(dest.getStopAndColor().Size(), 2);
    EXPECT_EQ(dest.GetGradient(), Paint::Radial);

    EXPECT_FLOAT_EQ(dest.GetRadialGradientPoint().scaleX, 4.0f);
    EXPECT_FLOAT_EQ(dest.GetRadialGradientPoint().scaleY, 5.0f);
    EXPECT_EQ(dest.GetPatternRepeatMode(), REPEAT_X);

    EXPECT_EQ(dest.GetShadowBlur(), 10);
    EXPECT_FLOAT_EQ(dest.GetShadowOffsetX(), 1.0f);
    EXPECT_FLOAT_EQ(dest.GetShadowOffsetY(), 2.0f);
    EXPECT_EQ(dest.GetShadowColor().full, Color::Gray().full);
    EXPECT_TRUE(dest.HaveShadow());

    EXPECT_FLOAT_EQ(dest.GetGlobalAlpha(), 0.5f);
    EXPECT_EQ(dest.GetGlobalCompositeOperation(), LIGHTER);
    EXPECT_TRUE(dest.HaveComposite());
    EXPECT_FLOAT_EQ(dest.GetRotateAngle(), 30.0f);

    float srcX = 5.0f, srcY = 6.0f;
    float dstX = srcX, dstY = srcY;
    source.GetTransAffine().Transform(&srcX, &srcY);
    dest.GetTransAffine().Transform(&dstX, &dstY);
    EXPECT_FLOAT_EQ(srcX, dstX);
    EXPECT_FLOAT_EQ(srcY, dstY);

    EXPECT_EQ(dest.GetFillingRule(), FILL_EVEN_ODD);
}

/**
 * @tc.name: PaintSvgTest_CreateLinearGradientDefaultScale_001
 * @tc.desc: Verify createLinearGradient initializes linear gradient scale to 1.0/1.0.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_CreateLinearGradientDefaultScale_001, TestSize.Level1)
{
    Paint paint;
    paint.createLinearGradient(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_FLOAT_EQ(paint.GetLinearGradientScaleX(), 1.0f);
    EXPECT_FLOAT_EQ(paint.GetLinearGradientScaleY(), 1.0f);
}

/**
 * @tc.name: PaintSvgTest_CreateRadialGradientDefaultScale_001
 * @tc.desc: Verify createRadialGradient initializes radial gradient scale to 1.0/1.0.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_CreateRadialGradientDefaultScale_001, TestSize.Level1)
{
    Paint paint;
    paint.createRadialGradient(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 5.0f);
    EXPECT_FLOAT_EQ(paint.GetRadialGradientPoint().scaleX, 1.0f);
    EXPECT_FLOAT_EQ(paint.GetRadialGradientPoint().scaleY, 1.0f);
}

/**
 * @tc.name: PaintSvgTest_SetLinearGradientScale_001
 * @tc.desc: Verify SetLinearGradientScale updates scale values.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_SetLinearGradientScale_001, TestSize.Level1)
{
    Paint paint;
    paint.createLinearGradient(0.0f, 0.0f, 10.0f, 10.0f);
    paint.SetLinearGradientScale(2.5f, 3.5f);
    EXPECT_FLOAT_EQ(paint.GetLinearGradientScaleX(), 2.5f);
    EXPECT_FLOAT_EQ(paint.GetLinearGradientScaleY(), 3.5f);
}

/**
 * @tc.name: PaintSvgTest_SetRadialGradientScale_001
 * @tc.desc: Verify SetRadialGradientScale updates radial gradient scale values.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_SetRadialGradientScale_001, TestSize.Level1)
{
    Paint paint;
    paint.createRadialGradient(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 5.0f);
    paint.SetRadialGradientScale(1.5f, 2.5f);
    EXPECT_FLOAT_EQ(paint.GetRadialGradientPoint().scaleX, 1.5f);
    EXPECT_FLOAT_EQ(paint.GetRadialGradientPoint().scaleY, 2.5f);
}

/**
 * @tc.name: PaintSvgTest_SetAndGetFillingRule_001
 * @tc.desc: Verify SetFillingRule and GetFillingRule round-trip.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_SetAndGetFillingRule_001, TestSize.Level1)
{
    Paint paint;
    EXPECT_EQ(paint.GetFillingRule(), FILL_NON_ZERO);
    paint.SetFillingRule(FILL_EVEN_ODD);
    EXPECT_EQ(paint.GetFillingRule(), FILL_EVEN_ODD);
}

/**
 * @tc.name: PaintSvgTest_ClearColorStops_001
 * @tc.desc: Verify ClearColorStops removes all color stops.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_ClearColorStops_001, TestSize.Level1)
{
    Paint paint;
    paint.createLinearGradient(0.0f, 0.0f, 10.0f, 10.0f);
    paint.addColorStop(0.0f, Color::Red());
    paint.addColorStop(1.0f, Color::Blue());
    EXPECT_EQ(paint.getStopAndColor().Size(), 2);
    paint.ClearColorStops();
    EXPECT_EQ(paint.getStopAndColor().Size(), 0);
}

/**
 * @tc.name: PaintSvgTest_RasterizerSetFillingRule_001
 * @tc.desc: Verify RasterizerScanlineAntialias::SetFillingRule does not crash and accepts both rules.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(PaintSvgTest, PaintSvgTest_RasterizerSetFillingRule_001, TestSize.Level1)
{
    RasterizerScanlineAntialias rasterizer;
    rasterizer.SetFillingRule(FILL_NON_ZERO);
    rasterizer.SetFillingRule(FILL_EVEN_ODD);
    EXPECT_TRUE(true);
}
#endif
} // namespace OHOS
