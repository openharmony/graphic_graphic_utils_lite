/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @file rasterizer_cells_antialias.h
 * @brief Defines Grating cell (anti aliasing)
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_RASTERIZER_CELLS_ANTIALIAS_H
#define GRAPHIC_LITE_RASTERIZER_CELLS_ANTIALIAS_H

#include <cstdlib>
#include <cstring>
#include <limits>

#include "gfx_utils/diagram/common/common_math.h"
#include "gfx_utils/diagram/vertexprimitive/geometry_plaindata_vector.h"

namespace OHOS {
// There is no constructor defined for pixel cells,
// which is to avoid the additional overhead of allocating cell arrays
struct CellBuildAntiAlias {
    int32_t x;
    int32_t y;
    int32_t cover;
    int32_t area;

    void Initial()
    {
        x = (std::numeric_limits<int32_t>::max)();
        y = (std::numeric_limits<int32_t>::max)();
        cover = 0;
        area = 0;
    }

    void Style(const CellBuildAntiAlias&) {}

    int32_t NotEqual(int32_t ex, int32_t ey, const CellBuildAntiAlias&) const
    {
        return (static_cast<uint32_t>(ex) - static_cast<uint32_t>(x))
                | (static_cast<uint32_t>(ey) - static_cast<uint32_t>(y));
    }
};

template <class Cell>
class RasterizerCellsAntiAlias {
    struct SortedYLevel {
        uint32_t start;
        uint32_t num;
    };

    /**
     * @brief Build the offset of 'cell unit', mask mask, cell pool capacity, etc
     * @since 1.0
     * @version 1.0
     */
    enum CellBlockScale {
        CELL_BLOCK_SHIFT = 12,
        CELL_BLOCK_SIZE = 1 << CELL_BLOCK_SHIFT,
        CELL_BLOCK_MASK = CELL_BLOCK_SIZE - 1,
        CELL_BLOCK_POOL = 256
    };

    enum DxLimit {
        DX_LIMIT = CONSTITUTION << POLY_SUBPIXEL_SHIFT
    };

public:
    using CellType = Cell;
    using SelfType = RasterizerCellsAntiAlias<Cell>;

    ~RasterizerCellsAntiAlias();

    /**
     * @brief RasterizerCellsAntiAlias Class constructor
     * initialization numBlocks_,maxBlocks_,currBlock_ etc
     * @since 1.0
     * @version 1.0
     */
    RasterizerCellsAntiAlias(uint32_t cellBlockLimit = 1024);

    /**
     * Reinitialize settings numBlocks_,maxBlocks_,currBlock_ etc。
     * @since 1.0
     * @version 1.0
     */
    void Reset();
    void Style(const CellType& styleCell);

    /**
     * @brief According to the incoming 2 coordinate points (both with sub pixels),
     * The process of constructing rasterized cell points is from y to X.
     * @since 1.0
     * @version 1.0
     */
    void LineOperate(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    void LineOperateVerticalLine();

    /**
     * @brief The rasterization process builds the coordinate range of the entity.
     * @since 1.0
     * @version 1.0
     */
    int32_t GetMinX() const
    {
        return minX_;
    }
    int32_t GetMinY() const
    {
        return minY_;
    }
    int32_t GetMaxX() const
    {
        return maxX_;
    }
    int32_t GetMaxY() const
    {
        return maxY_;
    }

    /**
     * @brief In the rasterization process, all cells are rasterized according to
     * Sort from left to right and from top to bottom.
     * @since 1.0
     * @version 1.0
     */
    void SortAllCells();

    uint32_t GetTotalCells() const
    {
        return numCells_;
    }

    /**
     * @brief In the process of rasterization, it is calculated according to the coordinate height of Y
     * Total number of cells.
     * @since 1.0
     * @version 1.0
     */
    uint32_t ScanlineNumCells(uint32_t yLevel) const
    {
        return sortedY_[yLevel - minY_].num;
    }

    /**
     * @brief In the process of rasterization, it is calculated according to the coordinate height of Y
     * The first address of the sorted cells array.
     * @since 1.0
     * @version 1.0
     */
    const CellType * const *ScanlineCells(uint32_t yLevel) const
    {
        return sortedCells_.Data() + sortedY_[yLevel - minY_].start;
    }

    bool GetSorted() const
    {
        return sorted_;
    }

private:
    RasterizerCellsAntiAlias(const SelfType&);
    const SelfType& operator=(const SelfType&);

    /**
     * @brief Set the current cell during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void SetCurrentCell(int32_t x, int32_t y);

    void OutLineLegal(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    /**
     * @brief Add the current cell during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void AddCurrentCell();

    /**
     * @brief n the rasterization process, the horizontal direction is
     * from x1 to x2 according to the coordinate height value of ey,
     * The filling process of cell cells longitudinally from sub-pixel mask y1 to sub-pixel mask y2.
     * @since 1.0
     * @version 1.0
     */
    void RenderHorizonline(int32_t ey, int32_t x1, int32_t submaskFlagsY1, int32_t x2, int32_t submaskFlagsY2);

    /**
     * @brief Allocate array space for cells during rasterization.
     * @since 1.0
     * @version 1.0
     */
    void AllocateBlock();

private:
    uint32_t numBlocks_;
    uint32_t maxBlocks_;
    uint32_t currBlock_;
    uint32_t numCells_;
    uint32_t cellBlockLimit_;
    CellType** cells_;
    CellType* currCellPtr_;
    GeometryPlaindataVector<CellType*> sortedCells_;
    GeometryPlaindataVector<SortedYLevel> sortedY_;
    CellType currCell_;
    CellType styleCell_;
    int32_t minX_;
    int32_t minY_;
    int32_t maxX_;
    int32_t maxY_;
    bool sorted_;
};

class ScanlineHitRegionMeasure {
public:
    ScanlineHitRegionMeasure(int32_t x) : xCoordinate_(x), hitMeasureFlags_(false) {}

    void AddCellInContainer(int32_t x, int32_t)
    {
        if (xCoordinate_ == x) {
            hitMeasureFlags_ = true;
        }
    }
    void AddSpanInContainer(int32_t x, int32_t len, int32_t)
    {
        if (xCoordinate_ >= x && xCoordinate_ < x + len) {
            hitMeasureFlags_ = true;
        }
    }
    uint32_t GetNumberSpans() const
    {
        return 1;
    }
    bool GetHitMeasure() const
    {
        return hitMeasureFlags_;
    }

private:
    int32_t xCoordinate_;
    bool hitMeasureFlags_;
};

template <class Cell>
RasterizerCellsAntiAlias<Cell>::~RasterizerCellsAntiAlias()
{
    if (numBlocks_) {
        CellType** ptr = cells_ + numBlocks_ - 1;
        while (numBlocks_--) {
            GeometryArrayAllocator<CellType>::Deallocate(*ptr, CELL_BLOCK_SIZE);
            ptr--;
        }
        GeometryArrayAllocator<CellType*>::Deallocate(cells_, maxBlocks_);
    }
}

/**
 * @brief RasterizerCellsAntiAlias Class constructor
 * initialization numBlocks_,maxBlocks_,currBlock_ Other attributes
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
RasterizerCellsAntiAlias<Cell>::RasterizerCellsAntiAlias(uint32_t cellBlockLimit)
    : numBlocks_(0),
      maxBlocks_(0),
      currBlock_(0),
      numCells_(0),
      cellBlockLimit_(cellBlockLimit),
      cells_(0),
      currCellPtr_(0),
      sortedCells_(),
      sortedY_(),
      minX_((std::numeric_limits<int32_t>::max)()),
      minY_((std::numeric_limits<int32_t>::max)()),
      maxX_((std::numeric_limits<int32_t>::min)()),
      maxY_((std::numeric_limits<int32_t>::min)()),
      sorted_(false)
{
    styleCell_.Initial();
    currCell_.Initial();
}

/**
 * Reinitialize settings numBlocks_,maxBlocks_,currBlock_ and other attributes.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::Reset()
{
    numCells_ = 0;
    currBlock_ = 0;
    currCell_.Initial();
    styleCell_.Initial();
    sorted_ = false;
    minX_ = (std::numeric_limits<int32_t>::max)();
    minY_ = (std::numeric_limits<int32_t>::max)();
    maxX_ = (std::numeric_limits<int32_t>::min)();
    maxY_ = (std::numeric_limits<int32_t>::min)();
}

/**
 * @brief Add the current cell during rasterization.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::AddCurrentCell()
{
    bool areaCoverFlags = currCell_.area | currCell_.cover;
    if (areaCoverFlags) {
        // Reach CELL_BLOCK_MASK After the number of mask, re allocate memory
        if ((numCells_ & CELL_BLOCK_MASK) == 0) {
            // Exceeds the memory block size limit. The default is 1024 limit
            if (numBlocks_ >= cellBlockLimit_) {
                return;
            }
            AllocateBlock();
        }
        *currCellPtr_++ = currCell_;
        ++numCells_;
    }
}

/**
 * @brief Set the current cell during rasterization.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
inline void RasterizerCellsAntiAlias<Cell>::SetCurrentCell(int32_t x, int32_t y)
{
    if (currCell_.NotEqual(x, y, styleCell_)) {
        AddCurrentCell();
        currCell_.Style(styleCell_);
        currCell_.x = x;
        currCell_.y = y;
        currCell_.cover = 0;
        currCell_.area = 0;
    }
}

template <class Cell>
void RasterizerCellsAntiAlias<Cell>::OutLineLegal(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /**
     * outline range
     */
    if (x1 < minX_) {
        minX_ = x1;
    }
    if (x1 > maxX_) {
        maxX_ = x1;
    }
    if (y1 < minY_) {
        minY_ = y1;
    }
    if (y1 > maxY_) {
        maxY_ = y1;
    }
    if (x2 < minX_) {
        minX_ = x2;
    }
    if (x2 > maxX_) {
        maxX_ = x2;
    }
    if (y2 < minY_) {
        minY_ = y2;
    }
    if (y2 > maxY_) {
        maxY_ = y2;
    }
}

/**
 * @brief In the rasterization process, according to the coordinate height value of ey,
 * x1 to x2 in 1 / 256 pixel horizontally,
 * The filling process of cell cells longitudinally from sub-pixel mask y1 to sub-pixel mask y2.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::RenderHorizonline(
    int32_t ey, int32_t x1, int32_t polySubpixelMaskY1, int32_t x2, int32_t polySubpixelMaskY2)
{
    /**
     * Take out the mask value of the last 8 bits, namely the color mask,
     * from the points in units of 1 / 256 pixels
     */
    int32_t submaskFlagsX1 = x1 & POLY_SUBPIXEL_MASK;
    int32_t submaskFlagsX2 = x2 & POLY_SUBPIXEL_MASK;
    /**
     * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
     */
    int32_t pixelX1 = x1 >> POLY_SUBPIXEL_SHIFT;
    int32_t pixelX2 = x2 >> POLY_SUBPIXEL_SHIFT;

    int32_t delta, deltayMask, first;
    long long dx;
    int32_t increase, liftDxMask, modDxMask, remDxMask;
    /**
     * The color mask of the two points is the same. Add the settings directly and return.
     */
    if (polySubpixelMaskY2 == polySubpixelMaskY1) {
        SetCurrentCell(pixelX2, ey);
        return;
    }

    // The pixel coordinates of the two points are the same and are directly calculated as a cell.
    if (pixelX1 == pixelX2) {
        delta = polySubpixelMaskY2 - polySubpixelMaskY1;
        currCell_.cover += delta;
        currCell_.area += (submaskFlagsX1 + submaskFlagsX2) * delta;
        return;
    }
    // hline At the beginning of the process, the cells area adjacent to the same organization is rendered
    first = POLY_SUBPIXEL_SCALE;
    increase = 1;
    /**  Convert from submaskFlagsX1 to POLY_SUBPIXEL_SCALE to calculate deltax * deltay */
    deltayMask = (POLY_SUBPIXEL_SCALE - submaskFlagsX1) * (polySubpixelMaskY2 - polySubpixelMaskY1);
    dx = (long long)x2 - (long long)x1;
    if (dx < 0) {
        first = 0;
        increase = -1;
        dx = -dx;
        deltayMask = submaskFlagsX1 * (polySubpixelMaskY2 - polySubpixelMaskY1);
    }
    delta = static_cast<int32_t>(deltayMask / dx);
    modDxMask = static_cast<int32_t>(deltayMask % dx);
    if (modDxMask < 0) {
        modDxMask += dx;
        delta--;
    }
    /* submaskFlagsX1+ (0->first)过程 */
    currCell_.area += (submaskFlagsX1 + first) * delta;
    currCell_.cover += delta;
    pixelX1 += increase;
    SetCurrentCell(pixelX1, ey);
    polySubpixelMaskY1 += delta;
    if (pixelX1 != pixelX2) {
        /* delta_subpixel x（ 0 到 POLY_SUBPIXEL_SCALE）  到 ( delta_subpixel_scale_y + delta) */
        deltayMask = POLY_SUBPIXEL_SCALE * (polySubpixelMaskY2 - polySubpixelMaskY1 + delta);
        remDxMask = static_cast<int32_t>(deltayMask % dx);
        liftDxMask = static_cast<int32_t>(deltayMask / dx);
        if (remDxMask < 0) {
            liftDxMask--;
            remDxMask += dx;
        }
        modDxMask -= dx;
        while (pixelX1 != pixelX2) {
            delta = liftDxMask;
            modDxMask += remDxMask;
            if (modDxMask >= 0) {
                modDxMask -= dx;
                delta++;
            }
            currCell_.area += POLY_SUBPIXEL_SCALE * delta;
            currCell_.cover += delta;
            polySubpixelMaskY1 += delta;
            pixelX1 += increase;
            SetCurrentCell(pixelX1, ey);
        }
    }
    delta = polySubpixelMaskY2 - polySubpixelMaskY1;
    currCell_.cover += delta;
    /* From first to POLY_SUBPIXEL_SCALE procedure */
    currCell_.area += (submaskFlagsX2 + POLY_SUBPIXEL_SCALE - first) * delta;
}

template <class Cell>
inline void RasterizerCellsAntiAlias<Cell>::Style(const CellType& styleCell)
{
    styleCell_.style(styleCell);
}

/**
 * @brief According to the incoming 2 coordinate points (both with sub pixels),
 * The process of constructing rasterized cell points is from y to X.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::LineOperate(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    long long dx = (long long)x2 - (long long)x1;
    /**
     * If dx exceeds the limit, a compromise is adopted to calculate the line.
     */
    if (dx >= DX_LIMIT || dx <= -DX_LIMIT) {
        int32_t cx = static_cast<int32_t>(((long long)x1 + (long long)x2) >> 1);
        int32_t cy = static_cast<int32_t>(((long long)y1 + (long long)y2) >> 1);
        LineOperate(x1, y1, cx, cy);
        LineOperate(cx, cy, x2, y2);
    }
    /**
     * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
     */
    long long dy = (long long)y2 - (long long)y1;
    int32_t ex1 = x1 >> POLY_SUBPIXEL_SHIFT;
    int32_t ex2 = x2 >> POLY_SUBPIXEL_SHIFT;
    int32_t ey1 = y1 >> POLY_SUBPIXEL_SHIFT;
    int32_t ey2 = y2 >> POLY_SUBPIXEL_SHIFT;
    /**
     * Take out the mask value of the last 8 bits from
     * the points with 1 / 256 pixel as the unit, that is, the color mask
     */
    int32_t submaskFlagsY1 = y1 & POLY_SUBPIXEL_MASK;
    int32_t submaskFlagsY2 = y2 & POLY_SUBPIXEL_MASK;

    int32_t xFrom, xTo;
    int32_t remDyMask, modDyMask, liftDyMask, delta, first, increase;
    long long deltaxMask;

    OutLineLegal(ex1, ey1, ex2, ey2);
    SetCurrentCell(ex1, ey1);

    /**
     * If the Y values of the two points are the same, they will be directly rendered horizontally,
     * The horizontal coordinate spacing is from X1 - > x2 in 1 / 256 pixels,
     * Color mask spacing is from submaskFlagsY1 to submaskFlagsY2
     */
    if (ey1 == ey2) {
        RenderHorizonline(ey1, x1, submaskFlagsY1, x2, submaskFlagsY2);
        return;
    }
    /**
     * For the processing of vertical lines, start - > end cells are calculated, and then the line is calculated
     * The above general attribute area - > cover is for each y value,
     * There is only one cell, so it is no longer called RenderHorizonline()
     */
    increase = 1;
    //  Vertical line
    if (dx == 0) {
        /**
         * The coordinates of the first 24 bits are extracted from the points in units of 1 / 256 pixels
         */
        int32_t ex = x1 >> POLY_SUBPIXEL_SHIFT;
        /* Take out the number of decimal points and occupy 2 spaces */
        int32_t twoFx = (x1 - (ex << POLY_SUBPIXEL_SHIFT)) << 1;
        int32_t area;
        /* 256 */
        first = POLY_SUBPIXEL_SCALE;
        if (dy < 0) {
            first = 0;
            increase = -1;
        }
        xFrom = x1;
        /* From submaskFlagsY1 to first process */
        /* The color mask is from submaskFlagsY1->first */
        delta = first - submaskFlagsY1;
        currCell_.cover += delta;
        currCell_.area += twoFx * delta;
        ey1 += increase;
        SetCurrentCell(ex, ey1);
        /* The color mask is from (poly_subpixel_scale - first) -> first */
        delta = first + first - POLY_SUBPIXEL_SCALE;
        area = twoFx * delta;
        while (ey1 != ey2) {
            /* from poly_subpixel_scale - first to  first */
            currCell_.cover = delta;
            currCell_.area = area;
            ey1 += increase;
            SetCurrentCell(ex, ey1);
        }
        /* The color mask is from poly_subpixel_scale - first to  submaskFlagsY2 */
        delta = submaskFlagsY2 - POLY_SUBPIXEL_SCALE + first;
        currCell_.cover += delta;
        currCell_.area += twoFx * delta;
        return;
    }
    // ok, we have to render several hlines
    // dx* mask Difference
    /**
     * The color mask is from the 颜色mask是从submaskFlagsY1 to POLY_SUBPIXEL_SCALE Process of scale
     */
    deltaxMask = (POLY_SUBPIXEL_SCALE - submaskFlagsY1) * dx;
    first = POLY_SUBPIXEL_SCALE;
    if (dy < 0) {
        deltaxMask = submaskFlagsY1 * dx;
        first = 0;
        increase = -1;
        dy = -dy;
    }
    delta = static_cast<int32_t>(deltaxMask / dy);
    modDyMask = static_cast<int32_t>(deltaxMask % dy);
    if (modDyMask < 0) {
        delta--;
        modDyMask += dy;
    }
    xFrom = x1 + delta;
    RenderHorizonline(ey1, x1, submaskFlagsY1, xFrom, first);
    ey1 += increase;
    SetCurrentCell(xFrom >> POLY_SUBPIXEL_SHIFT, ey1);
    if (ey1 != ey2) {
        deltaxMask = POLY_SUBPIXEL_SCALE * dx;
        liftDyMask = static_cast<int32_t>(deltaxMask / dy);
        remDyMask = static_cast<int32_t>(deltaxMask % dy);
        if (remDyMask < 0) {
            liftDyMask--;
            remDyMask += dy;
        }
        modDyMask -= dy;
        while (ey1 != ey2) {
            delta = liftDyMask;
            modDyMask += remDyMask;
            if (modDyMask >= 0) {
                modDyMask -= dy;
                delta++;
            }
            xTo = xFrom + delta;
            RenderHorizonline(ey1, xFrom, POLY_SUBPIXEL_SCALE - first, xTo, first);
            xFrom = xTo;
            ey1 += increase;
            SetCurrentCell(xFrom >> POLY_SUBPIXEL_SHIFT, ey1);
        }
    }
    RenderHorizonline(ey1, xFrom, POLY_SUBPIXEL_SCALE - first, x2, submaskFlagsY2);
}

/**
 * @brief Allocate array space for cells during rasterization.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::AllocateBlock()
{
    if (currBlock_ >= numBlocks_) {
        if (numBlocks_ >= maxBlocks_) {
            CellType** newCells =
                GeometryArrayAllocator<CellType*>::Allocate(maxBlocks_ +
                                                            CELL_BLOCK_POOL);
            if (cells_) {
                if (memcpy_s(newCells, maxBlocks_ * sizeof(CellType*),
                             cells_, maxBlocks_ * sizeof(CellType*)) != EOK) {
                }
                GeometryArrayAllocator<CellType*>::Deallocate(cells_, maxBlocks_);
            }
            cells_ = newCells;
            maxBlocks_ += CELL_BLOCK_POOL;
        }
        cells_[numBlocks_++] = GeometryArrayAllocator<CellType>::Allocate(CELL_BLOCK_SIZE);
    }

    currCellPtr_ = cells_[currBlock_++];
}

/**
 * @brief The exchange of cells in the rasterization process.
 * @since 1.0
 * @version 1.0
 */
template <class T>
inline void SwapCells(T* oneCells, T* twoCells)
{
    T tempCells = *oneCells;
    *oneCells = *twoCells;
    *twoCells = tempCells;
}

template <class Cell>
void QsortCellsSweep(Cell*** base, Cell*** iIndex, Cell*** jIndex)
{
    /**
     * Sorting guarantees the value of * i < = * the value of base < = * the value of j
     */
    if ((**jIndex)->x < (**iIndex)->x) {
        SwapCells(*iIndex, *jIndex);
    }

    if ((**base)->x < (**iIndex)->x) {
        SwapCells(*base, *iIndex);
    }

    if ((**jIndex)->x < (**base)->x) {
        SwapCells(*base, *jIndex);
    }

    while (1) {
        int32_t x = (**base)->x;
        do {
            (*iIndex)++;
        } while ((**iIndex)->x < x);
        do {
            (*jIndex)--;
        } while (x < (**jIndex)->x);

        if ((*iIndex) > (*jIndex)) {
            break;
        }
        SwapCells(*iIndex, *jIndex);
    }
}

/**
 * @brief In the rasterization process, all cells are sorted quickly.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void QsortCells(Cell** start, uint32_t num)
{
    const int32_t QSORT_THRESHOLD = 9;
    const int32_t stackSize = 80;
    Cell** stack[stackSize];
    Cell*** top;
    Cell** limit;
    Cell** base;

    limit = start + num;
    base = start;
    top = stack;

    while (1) {
        int32_t len = int32_t(limit - base);

        Cell** iIndex;
        Cell** jIndex;
        Cell** pivot;

        if (len > QSORT_THRESHOLD) {
            /**
             * First exchange base + len / 2 as the pivot
             */
            pivot = base + len / TWO_TIMES;
            SwapCells(base, pivot);

            iIndex = base + 1;
            jIndex = limit - 1;

            QsortCellsSweep(&base, &iIndex, &jIndex);
            SwapCells(base, jIndex);
            if (jIndex - base > limit - iIndex) {
                top[0] = base;
                top[1] = jIndex;
                base = iIndex;
            } else {
                top[0] = iIndex;
                top[1] = limit;
                limit = jIndex;
            }
            top += TWO_STEP;
        } else {
            /**
             * When the sub-array becomes smaller, insert sort is performed using
             */
            jIndex = base;
            iIndex = jIndex + 1;
            QsortCellsFor(&iIndex, &jIndex, &limit, &base);
            if (top > stack) {
                top -= TWO_STEP;
                base = top[0];
                limit = top[1];
            } else {
                break;
            }
        }
    }
}

template <class Cell>
void QsortCellsFor(Cell*** iIndex, Cell*** jIndex, Cell*** limit, Cell*** base)
{
    for (; *iIndex < *limit; (*iIndex)++) {
        for (; (*jIndex)[1]->x < (**jIndex)->x; (*jIndex)--) {
            SwapCells((*jIndex) + 1, *jIndex);
            if ((*jIndex) == (*base)) {
                break;
            }
        }
        *jIndex = *iIndex;
    }
}

/**
 * @brief In the rasterization process, all cells are rasterized according to
 * Sort from left to right and from top to bottom.
 * @since 1.0
 * @version 1.0
 */
template <class Cell>
void RasterizerCellsAntiAlias<Cell>::SortAllCells()
{
    if (sorted_) {
        return; // Perform sort only the first time.
    }

    AddCurrentCell();
    currCell_.x = (std::numeric_limits<int32_t>::max)();
    currCell_.y = (std::numeric_limits<int32_t>::max)();
    currCell_.cover = 0;
    currCell_.area = 0;
    if (numCells_ == 0) {
        return;
    }

    // Allocate the array of cell pointers
    sortedCells_.Allocate(numCells_, CELLS_SIZE);

    // Allocate and zero the Y array
    sortedY_.Allocate(maxY_ - minY_ + 1, CELLS_SIZE);
    sortedY_.CleanData();

    // Create the Y-histogram (count the numbers of cells for each Y)
    CellType** blockPtr = cells_;
    CellType* cellPtr = nullptr;
    uint32_t nb = numCells_;
    uint32_t i = 0;
    while (nb) {
        cellPtr = *blockPtr++;
        i = (nb > CELL_BLOCK_SIZE) ? uint32_t(CELL_BLOCK_SIZE) : nb;
        nb -= i;
        while (i--) {
            sortedY_[cellPtr->y - minY_].start++;
            ++cellPtr;
        }
    }

    // Convert the Y-histogram into the array of starting indexes
    uint32_t start = 0;
    for (i = 0; i < sortedY_.GetSize(); i++) {
        uint32_t v = sortedY_[i].start;
        sortedY_[i].start = start;
        start += v;
    }

    // Fill the cell pointer array sorted by Y
    blockPtr = cells_;
    nb = numCells_;
    while (nb) {
        cellPtr = *blockPtr++;
        i = (nb > CELL_BLOCK_SIZE) ? uint32_t(CELL_BLOCK_SIZE) : nb;
        nb -= i;
        while (i--) {
            SortedYLevel& currY = sortedY_[cellPtr->y - minY_];
            sortedCells_[currY.start + currY.num] = cellPtr;
            ++currY.num;
            ++cellPtr;
        }
    }

    // Finally arrange the X-arrays
    for (i = 0; i < sortedY_.GetSize(); i++) {
        const SortedYLevel& currY = sortedY_[i];
        if (currY.num) {
            QsortCells(sortedCells_.Data() + currY.start, currY.num);
        }
    }
    sorted_ = true;
}
} // namespace OHOS
#endif
