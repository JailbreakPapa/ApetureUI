#include <Core/CorePCH.h>

#include <Core/World/SpatialData.h>

wdHybridArray<wdSpatialData::CategoryData, 32>& wdSpatialData::GetCategoryData()
{
  static wdHybridArray<wdSpatialData::CategoryData, 32> CategoryData;
  return CategoryData;
}

// static
wdSpatialData::Category wdSpatialData::RegisterCategory(wdStringView sCategoryName, const wdBitflags<Flags>& flags)
{
  Category oldCategory = FindCategory(sCategoryName);
  if (oldCategory != wdInvalidSpatialDataCategory)
  {
    WD_ASSERT_DEV(GetCategoryFlags(oldCategory) == flags, "Category registered with different flags");
    return oldCategory;
  }

  if (GetCategoryData().GetCount() == 32)
  {
    WD_REPORT_FAILURE("Too many spatial data categories");
    return wdInvalidSpatialDataCategory;
  }

  Category newCategory = Category(GetCategoryData().GetCount());

  auto& data = GetCategoryData().ExpandAndGetRef();
  data.m_sName.Assign(sCategoryName);
  data.m_Flags = flags;

  return newCategory;
}

// static
wdSpatialData::Category wdSpatialData::FindCategory(wdStringView sCategoryName)
{
  wdTempHashedString categoryName(sCategoryName);

  for (wdUInt32 uiCategoryIndex = 0; uiCategoryIndex < GetCategoryData().GetCount(); ++uiCategoryIndex)
  {
    if (GetCategoryData()[uiCategoryIndex].m_sName == categoryName)
      return Category(uiCategoryIndex);
  }

  return wdInvalidSpatialDataCategory;
}

// static
const wdBitflags<wdSpatialData::Flags>& wdSpatialData::GetCategoryFlags(Category category)
{
  return GetCategoryData()[category.m_uiValue].m_Flags;
}

//////////////////////////////////////////////////////////////////////////

wdSpatialData::Category wdDefaultSpatialDataCategories::RenderStatic = wdSpatialData::RegisterCategory("RenderStatic", wdSpatialData::Flags::None);
wdSpatialData::Category wdDefaultSpatialDataCategories::RenderDynamic = wdSpatialData::RegisterCategory("RenderDynamic", wdSpatialData::Flags::FrequentChanges);
wdSpatialData::Category wdDefaultSpatialDataCategories::OcclusionStatic = wdSpatialData::RegisterCategory("OcclusionStatic", wdSpatialData::Flags::None);
wdSpatialData::Category wdDefaultSpatialDataCategories::OcclusionDynamic = wdSpatialData::RegisterCategory("OcclusionDynamic", wdSpatialData::Flags::FrequentChanges);


WD_STATICLINK_FILE(Core, Core_World_Implementation_SpatialData);
