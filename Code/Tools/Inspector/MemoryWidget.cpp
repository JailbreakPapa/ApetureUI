#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <GuiFoundation/GuiFoundationDLL.h>
#include <Inspector/MemoryWidget.moc.h>
#include <QGraphicsPathItem>
#include <QGraphicsView>
#include <QMenu>

wdQtMemoryWidget* wdQtMemoryWidget::s_pWidget = nullptr;

namespace MemoryWidgetDetail
{
  static QColor s_Colors[wdQtMemoryWidget::s_uiMaxColors] = {
    QColor(255, 106, 0),   // orange
    QColor(182, 255, 0),   // lime green
    QColor(255, 0, 255),   // pink
    QColor(0, 148, 255),   // light blue
    QColor(255, 0, 0),     // red
    QColor(0, 255, 255),   // turquoise
    QColor(217, 128, 255), // purple
    QColor(128, 147, 255), // dark blue
    QColor(164, 128, 255), // lilac
  };
}

void FormatSize(wdStringBuilder& s, const char* szPrefix, wdUInt64 uiSize)
{
  if (uiSize < 1024)
    s.Format("{0}{1} Bytes", szPrefix, uiSize);
  else if (uiSize < 1024 * 1024)
    s.Format("{0}{1} KB", szPrefix, wdArgF(uiSize / 1024.0, 1));
  else if (uiSize < 1024 * 1024 * 1024)
    s.Format("{0}{1} MB", szPrefix, wdArgF(uiSize / 1024.0 / 1024.0, 2));
  else
    s.Format("{0}{1} GB", szPrefix, wdArgF(uiSize / 1024.0 / 1024.0 / 1024.0, 2));
}

wdQtMemoryWidget::wdQtMemoryWidget(QWidget* pParent)
  : ads::CDockWidget("Memory Widget", pParent)
{
  s_pWidget = this;

  setupUi(this);
  setWidget(MemoryWidgetFrame);

  {
    wdQtScopedUpdatesDisabled _1(ComboTimeframe);

    ComboTimeframe->addItem("Timeframe: 10 seconds");
    ComboTimeframe->addItem("Timeframe: 30 seconds");
    ComboTimeframe->addItem("Timeframe: 1 minute");
    ComboTimeframe->addItem("Timeframe: 2 minutes");
    ComboTimeframe->addItem("Timeframe: 5 minutes");
    ComboTimeframe->addItem("Timeframe: 10 minutes");
    ComboTimeframe->setCurrentIndex(2);
  }

  m_pPathMax = m_Scene.addPath(QPainterPath(), QPen(QBrush(QColor(255, 255, 255)), 0));

  for (wdUInt32 i = 0; i < s_uiMaxColors; ++i)
    m_pPath[i] = m_Scene.addPath(QPainterPath(), QPen(QBrush(MemoryWidgetDetail::s_Colors[i]), 0));

  QTransform t = UsedMemoryView->transform();
  t.scale(1, -1);
  UsedMemoryView->setTransform(t);

  UsedMemoryView->setScene(&m_Scene);

  UsedMemoryView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  UsedMemoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  UsedMemoryView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  // UsedMemoryView->setMaximumHeight(100);

  ListAllocators->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ListAllocators, &QTreeView::customContextMenuRequested, this, &wdQtMemoryWidget::CustomContextMenuRequested);

  ResetStats();
}

void wdQtMemoryWidget::ResetStats()
{
  m_AllocatorData.Clear();

  m_uiMaxSamples = 3000;
  m_uiDisplaySamples = 5 * 60; // 5 samples per second, 60 seconds
  m_uiColorsUsed = 1;
  m_bAllocatorsChanged = true;

  m_Accu = AllocatorData();

  ListAllocators->clear();
}

void wdQtMemoryWidget::UpdateStats()
{
  if (!isVisible())
    return;

  if (!wdTelemetry::IsConnectedToServer())
  {
    ListAllocators->setEnabled(false);

    LabelNumAllocs->setText(QString::fromUtf8("Alloc Counter: N/A"));
    LabelNumLiveAllocs->setText(QString::fromUtf8("Live Allocs: N/A"));

    LabelCurMemory->setText(QString::fromUtf8("Cur: N/A"));
    LabelMaxMemory->setText(QString::fromUtf8("Max: N/A"));
    LabelMinMemory->setText(QString::fromUtf8("Min: N/A"));

    return;
  }

  ListAllocators->setEnabled(true);

  if (m_bAllocatorsChanged)
  {
    m_bAllocatorsChanged = false;

    wdQtScopedUpdatesDisabled _1(ListAllocators);
    wdQtScopedBlockSignals _2(ListAllocators);

    ListAllocators->clear();

    {
      m_Accu.m_pTreeItem = new QTreeWidgetItem();
      m_Accu.m_pTreeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      m_Accu.m_pTreeItem->setCheckState(0, m_Accu.m_bDisplay ? Qt::Checked : Qt::Unchecked);
      m_Accu.m_pTreeItem->setData(0, Qt::UserRole, wdUInt32(wdInvalidIndex));

      m_Accu.m_pTreeItem->setText(0, "<Accumulated>");
      m_Accu.m_pTreeItem->setForeground(0, QColor(255, 255, 255));

      ListAllocators->addTopLevelItem(m_Accu.m_pTreeItem);
    }

    for (auto it = m_AllocatorData.GetIterator(); it.IsValid(); ++it)
    {
      auto pParentData = m_AllocatorData.Find(it.Value().m_uiParentId);
      QTreeWidgetItem* pParent = pParentData.IsValid() ? pParentData.Value().m_pTreeItem : m_Accu.m_pTreeItem;

      QTreeWidgetItem* pItem = new QTreeWidgetItem(pParent);
      pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
      pItem->setCheckState(0, it.Value().m_bDisplay ? Qt::Checked : Qt::Unchecked);
      pItem->setData(0, Qt::UserRole, it.Key());

      pItem->setText(0, it.Value().m_sName.GetData());
      pItem->setForeground(0, MemoryWidgetDetail::s_Colors[it.Value().m_uiColor % s_uiMaxColors]);

      it.Value().m_pTreeItem = pItem;
    }

    ListAllocators->expandAll();
  }

  // once a second update the display of the allocators in the list
  if (wdTime::Now() - m_LastUpdatedAllocatorList > wdTime::Seconds(1))
  {
    m_LastUpdatedAllocatorList = wdTime::Now();

    for (auto it = m_AllocatorData.GetIterator(); it.IsValid(); ++it)
    {
      if (!it.Value().m_pTreeItem || it.Value().m_UsedMemory.IsEmpty())
        continue;

      wdStringBuilder sSize;
      FormatSize(sSize, "", it.Value().m_UsedMemory.PeekBack());

      wdStringBuilder sMaxSize;
      FormatSize(sMaxSize, "", it.Value().m_uiMaxUsedMemory);

      wdStringBuilder sText = it.Value().m_sName;
      sText.AppendFormat(" [{0}]", sSize);

      wdStringBuilder sTooltip;
      sTooltip.Format("<p>Allocator: <b>{}</b><br>Current Memory Used: <b>{}</b><br>Max Memory Used: <b>{}</b><br>Live Allocations: <b>{}</b><br>Allocations: "
                      "<b>{}</b><br>Deallocations: <b>{}</b><br>",
        it.Value().m_sName,
        sSize.GetData(), sMaxSize.GetData(), it.Value().m_uiLiveAllocs, it.Value().m_uiAllocs, it.Value().m_uiDeallocs);

      it.Value().m_pTreeItem->setText(0, sText.GetData());
      it.Value().m_pTreeItem->setToolTip(0, sTooltip.GetData());
    }

    if (m_Accu.m_pTreeItem && !m_Accu.m_UsedMemory.IsEmpty())
    {
      wdStringBuilder sSize;
      FormatSize(sSize, "", m_Accu.m_UsedMemory.PeekBack());

      wdStringBuilder sMaxSize;
      FormatSize(sMaxSize, "", m_Accu.m_uiMaxUsedMemory);

      wdStringBuilder sText = "<Accumulated>";
      sText.AppendFormat(" [{0}]", sSize);

      wdStringBuilder sTooltip;
      sTooltip.Format("<p>Current Memory Used: <b>{0}</b><br>Max Memory Used: <b>{1}</b><br>Live Allocations: <b>{2}</b><br>Allocations: "
                      "<b>{3}</b><br>Deallocations: <b>{4}</b><br>",
        sSize.GetData(), sMaxSize.GetData(), m_Accu.m_uiLiveAllocs, m_Accu.m_uiAllocs, m_Accu.m_uiDeallocs);

      m_Accu.m_pTreeItem->setText(0, sText.GetData());
      m_Accu.m_pTreeItem->setToolTip(0, sTooltip.GetData());
    }
  }

  if (wdTime::Now() - s_pWidget->m_LastUsedMemoryStored > wdTime::Milliseconds(200))
  {
    m_LastUsedMemoryStored = wdTime::Now();

    wdUInt64 uiSumMemory = 0;

    for (auto it = m_AllocatorData.GetIterator(); it.IsValid(); ++it)
    {
      // sometimes no data arrives in time (game is too slow)
      // in this case simply assume the stats have not changed
      if (!it.Value().m_bReceivedData && !it.Value().m_UsedMemory.IsEmpty())
        it.Value().m_uiMaxUsedMemoryRecently = it.Value().m_UsedMemory.PeekBack();

      uiSumMemory += it.Value().m_uiMaxUsedMemoryRecently;

      it.Value().m_UsedMemory.PushBack(it.Value().m_uiMaxUsedMemoryRecently);

      it.Value().m_uiMaxUsedMemoryRecently = 0;
      it.Value().m_bReceivedData = false;
    }
  }
  else
    return;

  QPainterPath pp[s_uiMaxColors];

  wdUInt32 uiMaxSamples = 0;

  wdUInt64 uiUsedMemory = 0;
  wdUInt64 uiLiveAllocs = 0;
  wdUInt64 uiAllocs = 0;
  wdUInt64 uiDeallocs = 0;
  wdUInt64 uiMinUsedMemory = 0xFFFFFFFFFFFFFFFFull;
  wdUInt64 uiMaxUsedMemory = 0;

  {
    m_Accu.m_UsedMemory.SetCount(m_uiDisplaySamples);

    for (wdUInt32 i = 0; i < m_uiDisplaySamples; ++i)
      m_Accu.m_UsedMemory[i] = 0;

    m_Accu.m_uiAllocs = 0;
    m_Accu.m_uiDeallocs = 0;
    m_Accu.m_uiLiveAllocs = 0;
    m_Accu.m_uiMaxUsedMemory = 0;
  }

  for (auto it = s_pWidget->m_AllocatorData.GetIterator(); it.IsValid(); ++it)
  {
    if (it.Value().m_UsedMemory.IsEmpty() || !it.Value().m_bDisplay)
      continue;

    wdUInt64 uiMinUsedMemoryThis = 0xFFFFFFFFFFFFFFFFull;
    wdUInt64 uiMaxUsedMemoryThis = 0;

    const wdUInt32 uiColorPath = it.Value().m_uiColor % s_uiMaxColors;
    WD_ASSERT_DEV(uiColorPath < s_uiMaxColors, "Invalid color index: {}", uiColorPath);

    uiUsedMemory += it.Value().m_UsedMemory.PeekBack();
    uiLiveAllocs += it.Value().m_uiLiveAllocs;
    uiAllocs += it.Value().m_uiAllocs;
    uiDeallocs += it.Value().m_uiDeallocs;

    if (it.Value().m_UsedMemory.GetCount() > m_uiMaxSamples)
      it.Value().m_UsedMemory.PopFront(it.Value().m_UsedMemory.GetCount() - m_uiMaxSamples);

    uiMaxSamples = wdMath::Max(uiMaxSamples, wdMath::Min(m_uiDisplaySamples, it.Value().m_UsedMemory.GetCount()));

    const wdUInt32 uiFirstSample =
      (it.Value().m_UsedMemory.GetCount() <= m_uiDisplaySamples) ? 0 : (it.Value().m_UsedMemory.GetCount() - m_uiDisplaySamples);
    const wdUInt32 uiStartPos =
      (it.Value().m_UsedMemory.GetCount() >= m_uiDisplaySamples) ? 0 : (m_uiDisplaySamples - it.Value().m_UsedMemory.GetCount());

    pp[uiColorPath].moveTo(QPointF(uiStartPos, it.Value().m_UsedMemory[uiFirstSample]));
    uiMinUsedMemoryThis = wdMath::Min(uiMinUsedMemoryThis, it.Value().m_UsedMemory[uiFirstSample]);
    uiMaxUsedMemoryThis = wdMath::Max(uiMaxUsedMemoryThis, it.Value().m_UsedMemory[uiFirstSample]);

    if (it.Value().m_uiParentId == wdInvalidIndex) // only accumulate top level allocators
    {
      m_Accu.m_uiAllocs += it.Value().m_uiAllocs;
      m_Accu.m_uiDeallocs += it.Value().m_uiDeallocs;
      m_Accu.m_uiLiveAllocs += it.Value().m_uiLiveAllocs;

      m_Accu.m_UsedMemory[uiStartPos] += it.Value().m_UsedMemory[uiFirstSample];
      m_Accu.m_uiMaxUsedMemory += it.Value().m_uiMaxUsedMemory;
    }

    for (wdUInt32 i = uiFirstSample + 1; i < it.Value().m_UsedMemory.GetCount(); ++i)
    {
      pp[uiColorPath].lineTo(QPointF(uiStartPos + i - uiFirstSample, it.Value().m_UsedMemory[i]));

      uiMinUsedMemoryThis = wdMath::Min(uiMinUsedMemoryThis, it.Value().m_UsedMemory[i]);
      uiMaxUsedMemoryThis = wdMath::Max(uiMaxUsedMemoryThis, it.Value().m_UsedMemory[i]);

      if (it.Value().m_uiParentId == wdInvalidIndex)
      {
        m_Accu.m_UsedMemory[uiStartPos + i - uiFirstSample] += it.Value().m_UsedMemory[i];
      }

      if (m_Accu.m_bDisplay)
        uiMaxUsedMemoryThis = wdMath::Max(uiMaxUsedMemoryThis, m_Accu.m_UsedMemory[uiStartPos + i - uiFirstSample]);
    }

    uiMinUsedMemory = wdMath::Min(uiMinUsedMemory, uiMinUsedMemoryThis);
    uiMaxUsedMemory = wdMath::Max(uiMaxUsedMemory, uiMaxUsedMemoryThis);
  }

  QPainterPath pMax;

  if (m_Accu.m_bDisplay)
  {
    pMax.moveTo(QPointF(0, m_Accu.m_UsedMemory[0]));

    for (wdUInt32 i = 1; i < m_Accu.m_UsedMemory.GetCount(); ++i)
      pMax.lineTo(QPointF(i, m_Accu.m_UsedMemory[i]));
  }

  m_pPathMax->setPath(pMax);

  for (wdUInt32 i = 0; i < s_uiMaxColors; ++i)
    m_pPath[i]->setPath(pp[i]);

  // round min and max to some power of two
  {
    uiMinUsedMemory = wdMath::PowerOfTwo_Floor(uiMinUsedMemory);
    uiMaxUsedMemory = wdMath::PowerOfTwo_Ceil(uiMaxUsedMemory);
  }

  {
    UsedMemoryView->setSceneRect(QRectF(0, uiMinUsedMemory, m_uiDisplaySamples - 1, uiMaxUsedMemory));
    UsedMemoryView->fitInView(QRectF(0, uiMinUsedMemory, m_uiDisplaySamples - 1, uiMaxUsedMemory));

    wdStringBuilder s;

    FormatSize(s, "Min: ", uiMinUsedMemory);
    LabelMinMemory->setText(QString::fromUtf8(s.GetData()));

    s.Format("<p>Recent Minimum Memory Usage:<br>{0} GB<br>{1} MB<br>{2} KB<br>{3} Byte</p>", wdArgF(uiMinUsedMemory / 1024.0 / 1024.0 / 1024.0, 2),
      wdArgF(uiMinUsedMemory / 1024.0 / 1024.0, 2), wdArgF(uiMinUsedMemory / 1024.0, 2), uiMinUsedMemory);
    LabelMinMemory->setToolTip(QString::fromUtf8(s.GetData()));

    FormatSize(s, "Max: ", uiMaxUsedMemory);
    LabelMaxMemory->setText(QString::fromUtf8(s.GetData()));

    s.Format("<p>Recent Maximum Memory Usage:<br>{0} GB<br>{1} MB<br>{2} KB<br>{3} Byte</p>", wdArgF(uiMaxUsedMemory / 1024.0 / 1024.0 / 1024.0, 2),
      wdArgF(uiMaxUsedMemory / 1024.0 / 1024.0, 2), wdArgF(uiMaxUsedMemory / 1024.0, 2), uiMaxUsedMemory);
    LabelMaxMemory->setToolTip(QString::fromUtf8(s.GetData()));

    const wdUInt64 uiCurUsedMemory = uiUsedMemory;

    FormatSize(s, "Sum: ", uiCurUsedMemory);
    LabelCurMemory->setText(QString::fromUtf8(s.GetData()));

    s.Format("<p>Current Memory Usage:<br>{0} GB<br>{1} MB<br>{2} KB<br>{3} Byte</p>", wdArgF(uiCurUsedMemory / 1024.0 / 1024.0 / 1024.0, 2),
      wdArgF(uiCurUsedMemory / 1024.0 / 1024.0, 2), wdArgF(uiCurUsedMemory / 1024.0, 2), uiCurUsedMemory);
    LabelCurMemory->setToolTip(QString::fromUtf8(s.GetData()));

    s.Format("Allocs: {0}", uiLiveAllocs);
    LabelNumLiveAllocs->setText(QString::fromUtf8(s.GetData()));

    s.Format("Counter: {0} / {1}", uiAllocs, uiDeallocs);
    LabelNumAllocs->setText(QString::fromUtf8(s.GetData()));

    s.Format("<p>Allocations: <b>{0}</b><br>Deallocations: <b>{1}</b></p>", uiAllocs, uiDeallocs);
    LabelNumAllocs->setToolTip(QString::fromUtf8(s.GetData()));
  }
}

void wdQtMemoryWidget::CustomContextMenuRequested(const QPoint& pos)
{
  QModelIndex CurrentIndex = ListAllocators->currentIndex();

  QMenu m;
  if (CurrentIndex.isValid())
  {
    m.addAction(actionEnableOnlyThis);
    m.addSeparator();
  }

  m.addAction(actionEnableAll);
  m.addAction(actionDisableAll);

  m.exec(ListAllocators->viewport()->mapToGlobal(pos));
}

void wdQtMemoryWidget::ProcessTelemetry(void* pUnuseed)
{
  if (s_pWidget == nullptr)
    return;

  wdTelemetryMessage Msg;

  while (wdTelemetry::RetrieveMessage(' MEM', Msg) == WD_SUCCESS)
  {
    if (Msg.GetMessageID() == 'BGN')
    {
      for (auto it : s_pWidget->m_AllocatorData)
      {
        it.Value().m_bStillInUse = false;
      }
    }

    if (Msg.GetMessageID() == 'END')
    {
      for (auto it = s_pWidget->m_AllocatorData.GetIterator(); it.IsValid();)
      {
        if (!it.Value().m_bStillInUse)
        {
          s_pWidget->m_bAllocatorsChanged = true;
          it = s_pWidget->m_AllocatorData.Remove(it);
        }
        else
        {
          ++it;
        }
      }
    }

    if (Msg.GetMessageID() == 'STAT')
    {
      wdString sAllocatorName;
      wdUInt32 uiAllocatorId;
      wdUInt32 uiParentId;

      wdAllocatorBase::Stats MemStat;
      Msg.GetReader() >> uiAllocatorId;
      Msg.GetReader() >> sAllocatorName;
      Msg.GetReader() >> uiParentId;
      Msg.GetReader() >> MemStat;

      AllocatorData& ad = s_pWidget->m_AllocatorData[uiAllocatorId];
      ad.m_bStillInUse = true;
      ad.m_bReceivedData = true;
      ad.m_sName = sAllocatorName;
      ad.m_uiParentId = uiParentId;

      if (ad.m_uiColor == 0xFF)
      {
        ad.m_uiColor = s_pWidget->m_uiColorsUsed;
        ++s_pWidget->m_uiColorsUsed;
        s_pWidget->m_bAllocatorsChanged = true;
      }

      ad.m_uiAllocs = MemStat.m_uiNumAllocations;
      ad.m_uiDeallocs = MemStat.m_uiNumDeallocations;
      ad.m_uiLiveAllocs = MemStat.m_uiNumAllocations - MemStat.m_uiNumDeallocations;
      ad.m_uiMaxUsedMemoryRecently = wdMath::Max(ad.m_uiMaxUsedMemoryRecently, MemStat.m_uiAllocationSize);
      ad.m_uiMaxUsedMemory = wdMath::Max(ad.m_uiMaxUsedMemory, MemStat.m_uiAllocationSize);
    }
  }
}

void wdQtMemoryWidget::on_ListAllocators_itemChanged(QTreeWidgetItem* item)
{
  if (item->data(0, Qt::UserRole).toUInt() == wdInvalidIndex)
  {
    m_Accu.m_bDisplay = (item->checkState(0) == Qt::Checked);
    return;
  }

  m_AllocatorData[item->data(0, Qt::UserRole).toUInt()].m_bDisplay = (item->checkState(0) == Qt::Checked);
}

void wdQtMemoryWidget::on_ComboTimeframe_currentIndexChanged(int index)
{
  const wdUInt32 uiSeconds[] = {
    10,
    30,
    60 * 1,
    60 * 2,
    60 * 5,
    60 * 10,
  };

  m_uiDisplaySamples = 5 * uiSeconds[index]; // 5 samples per second
}

void wdQtMemoryWidget::on_actionEnableOnlyThis_triggered(bool)
{
  on_actionDisableAll_triggered(false);

  m_AllocatorData[ListAllocators->currentItem()->data(0, Qt::UserRole).toUInt()].m_bDisplay = true;
}

void wdQtMemoryWidget::on_actionEnableAll_triggered(bool)
{
  m_bAllocatorsChanged = true;

  for (auto it : m_AllocatorData)
  {
    it.Value().m_bDisplay = true;
  }
}

void wdQtMemoryWidget::on_actionDisableAll_triggered(bool)
{
  m_bAllocatorsChanged = true;

  for (auto it : m_AllocatorData)
  {
    it.Value().m_bDisplay = false;
  }
}