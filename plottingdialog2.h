/*
 * 文件名: plottingdialog2.h
 * 文件作用: 压力-产量双坐标曲线配置对话框头文件
 * 功能描述:
 * 1. 左侧配置压力数据（点线样式）。
 * 2. 右侧配置产量数据（阶梯/折线/散点切换，动态样式显隐）。
 * 3. 样式设置采用图标+中文预览。
 * 4. [新增] 补充“在新建窗口显示”的功能接口。
 */

#ifndef PLOTTINGDIALOG2_H
#define PLOTTINGDIALOG2_H

#include <QDialog>
#include <QStandardItemModel>
#include <QColor>
#include <QMap>
#include <QComboBox>
#include "qcustomplot.h"

namespace Ui {
class PlottingDialog2;
}

class PlottingDialog2 : public QDialog
{
    Q_OBJECT

public:
    explicit PlottingDialog2(const QMap<QString, QStandardItemModel*>& models, QWidget *parent = nullptr);
    ~PlottingDialog2();

    // --- 获取曲线基础信息 ---
    QString getChartName() const;

    // --- 压力数据获取 ---
    QString getPressFileName() const;
    int getPressXCol() const;
    int getPressYCol() const;
    QString getPressLegend() const; // 默认为Y列名

    // 压力样式
    QCPScatterStyle::ScatterShape getPressShape() const;
    QColor getPressPointColor() const;
    Qt::PenStyle getPressLineStyle() const;
    QColor getPressLineColor() const;
    int getPressLineWidth() const;

    // --- 产量数据获取 ---
    QString getProdFileName() const;
    int getProdXCol() const;
    int getProdYCol() const;
    QString getProdLegend() const; // 默认为Y列名

    // 产量绘图类型: 0-阶梯图, 1-折线图, 2-散点图
    int getProdGraphType() const;

    // 产量样式
    QCPScatterStyle::ScatterShape getProdPointShape() const;
    QColor getProdPointColor() const;
    Qt::PenStyle getProdLineStyle() const;
    QColor getProdLineColor() const;
    int getProdLineWidth() const;

    // [新增] 是否在新建窗口显示
    bool isNewWindow() const;

private slots:
    // 文件选择改变
    void onPressFileChanged(int index);
    void onProdFileChanged(int index);

    // 产量绘图类型改变 (控制样式控件显隐)
    void onProdTypeChanged(int index);

private:
    Ui::PlottingDialog2 *ui;
    QMap<QString, QStandardItemModel*> m_dataMap;
    QStandardItemModel* m_pressModel;
    QStandardItemModel* m_prodModel;

    static int s_chartCounter;

    // --- 辅助函数 ---
    void populatePressColumns();
    void populateProdColumns();

    void setupStyleUI(); // 初始化所有下拉框
    void initColorComboBox(QComboBox* combo);
    QIcon createPointIcon(QCPScatterStyle::ScatterShape shape);
    QIcon createLineIcon(Qt::PenStyle style);
};

#endif // PLOTTINGDIALOG2_H
