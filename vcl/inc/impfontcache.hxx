/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_VCL_INC_IMPFONTCACHE_HXX
#define INCLUDED_VCL_INC_IMPFONTCACHE_HXX

#include <unordered_map>
#include <boost/functional/hash.hpp>

#include <o3tl/lru_map.hxx>
#include <tools/gen.hxx>
#include <vcl/vcllayout.hxx>

#include "fontselect.hxx"

class Size;
namespace vcl { class Font; }
class PhysicalFontCollection;

// TODO: closely couple with PhysicalFontCollection

struct GlpyhBoundRectCacheKey
{
    const LogicalFontInstance* m_pFont;
    const sal_GlyphId m_nId;

    GlpyhBoundRectCacheKey(const LogicalFontInstance* pFont, sal_GlyphId nID)
        : m_pFont(pFont), m_nId(nID)
    {}

    bool operator==(GlpyhBoundRectCacheKey const& aOther) const
    { return m_pFont == aOther.m_pFont && m_nId == aOther.m_nId; }
};

struct GlpyhBoundRectCacheHash
{
    std::size_t operator()(GlpyhBoundRectCacheKey const& aCache) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, aCache.m_pFont);
        boost::hash_combine(seed, aCache.m_nId);
        return seed;
    }
};

typedef o3tl::lru_map<GlpyhBoundRectCacheKey, tools::Rectangle,
                      GlpyhBoundRectCacheHash> GlpyhBoundRectCache;
typedef GlpyhBoundRectCache::key_value_pair_t GlpyhBoundRectCachePair;

class ImplFontCache
{
private:
    // cache of recently used font instances
    struct IFSD_Equal { bool operator()( const FontSelectPattern&, const FontSelectPattern& ) const; };
    struct IFSD_Hash { size_t operator()( const FontSelectPattern& ) const; };
    typedef o3tl::lru_map<FontSelectPattern, rtl::Reference<LogicalFontInstance>, IFSD_Hash, IFSD_Equal> FontInstanceList;
    typedef FontInstanceList::key_value_pair_t FontInstanceListPair;

    LogicalFontInstance* mpLastHitCacheEntry; ///< keeps the last hit cache entry
    FontInstanceList maFontInstanceList;
    GlpyhBoundRectCache m_aBoundRectCache;

    rtl::Reference<LogicalFontInstance> GetFontInstance(PhysicalFontCollection const*, FontSelectPattern&);

public:
    ImplFontCache();
    ~ImplFontCache();

    rtl::Reference<LogicalFontInstance> GetFontInstance( PhysicalFontCollection const *,
                             const vcl::Font&, const Size& rPixelSize, float fExactHeight);
    rtl::Reference<LogicalFontInstance> GetGlyphFallbackFont( PhysicalFontCollection const *, FontSelectPattern&,
                            LogicalFontInstance* pLogicalFont,
                            int nFallbackLevel, OUString& rMissingCodes );

    bool GetCachedGlyphBoundRect(LogicalFontInstance *, sal_GlyphId, tools::Rectangle &);
    void CacheGlyphBoundRect(LogicalFontInstance *, sal_GlyphId, tools::Rectangle &);

    void                Invalidate();
};

#endif // INCLUDED_VCL_INC_IMPFONTCACHE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
