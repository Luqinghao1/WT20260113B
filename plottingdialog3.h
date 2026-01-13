/*
 * 文件名: plottingdialog3.h
 * 文件作用: 试井分析曲线（双对数曲线）配置对话框头文件
 * 功能描述:
 * 1. 界面分为数据设置、计算设置、样式设置三部分。
 * 2. 计算设置中，初始压力根据试井类型（压降/压恢）动态启用/禁用，并自动获取默认值。
 * 3. 样式设置支持压差和导数曲线的独立配置（包含线宽）。
 * 4. 样式控件采用图标可视化显示。
 */

#ifndef PLOTTINGDIALOG3_H
#define PLOTTINGDIALOG3_H

#include <QDialog>
#include <QStandardItemModel>
#include <QColor>
#include <QMap>
#include <QComboBox>
#include "qcustomplot.h"

namespace Ui {
class PlottingDialog3;
}

class PlottingDialog3 : public QDialog
{
    Q_OBJECT

public:
    enum TestType {
        Drawdown,   // 压力降落试井
        Buildup     // 压力恢复试井
    };

    explicit PlottingDialog3(const QMap<QString, QStandardItemModel*>& models, QWidget *parent = nullptr);
    ~PlottingDialog3();

    // --- 基础数据接口 ---
    QString getCurveName() const;
    QString getSelectedFileName() const;

    // 获取数据列索引
    int getTimeColumn() const;
    int getPressureColumn() const;

    // --- 计算参数接口 ---
    TestType getTestType() const;
    double getInitialPressure() const;
    double getLSpacing() const;
    bool isSmoothEnabled() const;
    int getSmoothFactor() const;

    // --- 坐标轴标签默认值 (无需UI输入) ---
    QString getXLabel() const { return "dt (h)"; }
    QString getYLabel() const { return "Delta P / Derivative (MPa)"; }

    // --- 图例默认值 ---
    QString getPressLegend() const { return "Delta P"; }
    QString getDerivLegend() const { return "Derivative"; }

    // --- 压差曲线样式接口 ---
    QCPScatterStyle::ScatterShape getPressShape() const;
    QColor getPressPointColor() const;
    Qt::PenStyle getPressLineStyle() const;
    QColor getPressLineColor() const;
    int getPressLineWidth() const;

    // --- 导数曲线样式接口 ---
    QCPScatterStyle::ScatterShape getDerivShape() const;
    QColor getDerivPointColor() const;
    Qt::PenStyle getDerivLineStyle() const;
    QColor getDerivLineColor() const;
    int getDerivLineWidth() const;

    // 是否在新建窗口显示
    bool isNewWindow() const;

private slots:
    // 文件切换
    void onFileChanged(int index);
    // 压力列切换 (用于更新初始压力默认值)
    void onPressureColumnChanged(int index);

    // 试井类型切换 (控制初始压力输入框状态)
    void onTestTypeChanged();

    // 平滑开关切换
    void onSmoothToggled(bool checked);

private:
    Ui::PlottingDialog3 *ui;
    QMap<QString, QStandardItemModel*> m_dataMap;
    QStandardItemModel* m_currentModel;

    static int s_counter;

    // --- 辅助函数 ---
    void populateComboBoxes();
    void updateInitialPressureDefault(); // 更新初始压力默认值

    void setupStyleUI(); // 初始化样式下拉框
    void initColorComboBox(QComboBox* combo);
    QIcon createPointIcon(QCPScatterStyle::ScatterShape shape);
    QIcon createLineIcon(Qt::PenStyle style);
};

#endif // PLOTTINGDIALOG3_H
