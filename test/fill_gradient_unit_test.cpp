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

#include "gfx_utils/diagram/spancolorfill/fill_gradient_svg.h"

#include <gtest/gtest.h>

#if defined(FEATURE_COMPONENT_SVG) && FEATURE_COMPONENT_SVG
#include "gfx_utils/diagram/spancolorfill/fill_gradient_lut.h"
#include "gfx_utils/diagram/spancolorfill/fill_interpolator.h"
#include "gfx_utils/trans_affine.h"
#endif

using namespace testing::ext;
namespace OHOS {
class FillGradientSvgTest : public testing::Test {
};

#if defined(FEATURE_COMPONENT_SVG) && FEATURE_COMPONENT_SVG
/**
 * @tc.name: FillGradientSvgTest_DitherColorChannel_001
 * @tc.desc: Verify DitherColorChannel clamps values into [0, 255] and adds threshold.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_DitherColorChannel_001, TestSize.Level1)
{
    EXPECT_EQ(DitherColorChannel(100.0f, 0.0f), static_cast<uint8_t>(100));
    EXPECT_EQ(DitherColorChannel(254.6f, 0.5f), static_cast<uint8_t>(255));
    EXPECT_EQ(DitherColorChannel(0.4f, 0.0f), static_cast<uint8_t>(0));
    EXPECT_EQ(DitherColorChannel(-5.0f, 0.0f), static_cast<uint8_t>(0));
    EXPECT_EQ(DitherColorChannel(300.0f, 0.0f), static_cast<uint8_t>(255));
}

/**
 * @tc.name: FillGradientSvgTest_GradientLinearCalculate_001
 * @tc.desc: Verify GradientLinearCalculateSvg::Calculate clamps index into [0, size - 1].
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_GradientLinearCalculate_001, TestSize.Level1)
{
    GradientLinearCalculateSvg linear;
    EXPECT_EQ(linear.Calculate(0, 0, 0, 1, 10), 0);
    EXPECT_EQ(linear.Calculate(5, 0, 0, 10, 10), 5);
    EXPECT_EQ(linear.Calculate(10, 0, 0, 10, 10), 9);
    EXPECT_EQ(linear.Calculate(-1, 0, 0, 10, 10), 0);
    EXPECT_EQ(linear.Calculate(0, 0, 0, 0, 10), 0);
}

/**
 * @tc.name: FillGradientSvgTest_GradientLinearCalculateFloat_001
 * @tc.desc: Verify GradientLinearCalculateSvg::CalculateFloat returns proper t and handles edge cases.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_GradientLinearCalculateFloat_001, TestSize.Level1)
{
    GradientLinearCalculateSvg linear;
    EXPECT_FLOAT_EQ(linear.CalculateFloat(0, 0, 0, 10, 10), 0.0f);
    EXPECT_FLOAT_EQ(linear.CalculateFloat(5, 0, 0, 10, 10), 5.0f);
    EXPECT_FLOAT_EQ(linear.CalculateFloat(0, 0, 0, 0, 10), 0.0f);
    EXPECT_FLOAT_EQ(linear.CalculateFloat(5, 0, 0, 10, 1), 0.0f);
}

/**
 * @tc.name: FillGradientSvgTest_GradientRadialCalculate_001
 * @tc.desc: Verify GradientRadialCalculateSvg::Calculate returns clamped index.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_GradientRadialCalculate_001, TestSize.Level1)
{
    GradientRadialCalculateSvg radial(10.0f, 0.0f, 0.0f);
    EXPECT_EQ(radial.Calculate(0, 0, 0, 10, 10), 0);
    EXPECT_EQ(radial.Calculate(5, 0, 0, 10, 10), 5);
    EXPECT_EQ(radial.Calculate(10, 0, 0, 10, 10), 9);
    EXPECT_EQ(radial.Calculate(-5, 0, 0, 10, 10), 5);
    EXPECT_EQ(radial.Calculate(100, 0, 0, 10, 10), 9);
    EXPECT_EQ(radial.Calculate(0, 0, 5, 5, 10), 0);
}

/**
 * @tc.name: FillGradientSvgTest_GradientRadialCalculateFloat_001
 * @tc.desc: Verify GradientRadialCalculateSvg::CalculateFloat returns non-negative t and handles small size.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_GradientRadialCalculateFloat_001, TestSize.Level1)
{
    GradientRadialCalculateSvg radial(10.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(radial.CalculateFloat(0, 0, 0, 10, 10), 0.0f);
    EXPECT_GE(radial.CalculateFloat(10, 0, 0, 10, 10), 0.0f);
    EXPECT_FLOAT_EQ(radial.CalculateFloat(0, 0, 0, 10, 1), 0.0f);
    EXPECT_FLOAT_EQ(radial.CalculateFloat(0, 0, 5, 5, 10), -50.0f);
}

/**
 * @tc.name: FillGradientSvgTest_FillGradientGenerate_001
 * @tc.desc: Verify FillGradientSvg::Generate produces expected span colors.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_FillGradientGenerate_001, TestSize.Level1)
{
    TransAffine identity;
    FillInterpolator interpolator(identity);
    GradientLinearCalculateSvg gradient;
    FillGradientLut lut;

    constexpr uint8_t RED_VALUE = 200;
    constexpr uint8_t GREEN_VALUE = 100;
    constexpr uint8_t BLUE_VALUE = 50;
    constexpr uint8_t ALPHA_VALUE = 255;
    Rgba8T color(RED_VALUE, GREEN_VALUE, BLUE_VALUE, ALPHA_VALUE);
    lut.AddColor(0.0f, color);
    lut.AddColor(1.0f, color);
    lut.BuildLut();

    FillGradientSvg fill(interpolator, gradient, lut, 0.0f, 1.0f);
    Rgba8T span[4];
    fill.Generate(span, 0, 0, 4);

    for (int32_t i = 0; i < 4; i++) {
        EXPECT_EQ(span[i].red, RED_VALUE);
        EXPECT_EQ(span[i].green, GREEN_VALUE);
        EXPECT_EQ(span[i].blue, BLUE_VALUE);
        EXPECT_EQ(span[i].alpha, ALPHA_VALUE);
    }
}

/**
 * @tc.name: FillGradientSvgTest_FillGradientGenerateClamp_001
 * @tc.desc: Verify FillGradientSvg::Generate clamps out-of-range LUT indices without crashing.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQC
 */
HWTEST_F(FillGradientSvgTest, FillGradientSvgTest_FillGradientGenerateClamp_001, TestSize.Level1)
{
    TransAffine identity;
    FillInterpolator interpolator(identity);
    GradientLinearCalculateSvg gradient;
    FillGradientLut lut;

    Rgba8T color1(255, 0, 0, 255);
    Rgba8T color2(0, 255, 0, 255);
    lut.AddColor(0.0f, color1);
    lut.AddColor(1.0f, color2);
    lut.BuildLut();

    FillGradientSvg fill(interpolator, gradient, lut, 0.0f, 1.0f);
    Rgba8T span[1];
    fill.Generate(span, -10, 0, 1);
    EXPECT_EQ(span[0].alpha, 255);
    fill.Generate(span, 300, 0, 1);
    EXPECT_EQ(span[0].alpha, 255);
}
#endif
} // namespace OHOS
