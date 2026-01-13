/*
 * 文件名: plottingdialog1.h
 * 文件作用: 新建单一曲线配置对话框头文件
 * 功能描述:
 * 1. 设置曲线基础信息（名称、数据源）。
 * 2. 选择X/Y数据列。
 * 3. [修改] 样式设置：
 * - 纯中文界面，移除了所有英文说明。
 * - 点形状和线类型下拉框增加所见即所得的图标预览。
 * - 颜色下拉框继续保持颜色块预览。
 */

#ifndef PLOTTINGDIALOG1_H
#define PLOTTINGDIALOG1_H

#include <QDialog>
#include <QStandardItemModel>
#include <QColor>
#include <QMap>
#include <QComboBox>
#include "qcustomplot.h"

namespace Ui {
class PlottingDialog1;
}

class PlottingDialog1 : public QDialog
{
    Q_OBJECT

public:
    // 构造函数接收所有数据模型的映射表
    explicit PlottingDialog1(const QMap<QString, QStandardItemModel*>& models, QWidget *parent = nullptr);
    ~PlottingDialog1();

    // --- 获取用户配置 ---
    QString getCurveName() const;
    QString getLegendName() const;      // 图例名称默认为 Y 轴列名
    QString getSelectedFileName() const;// 获取用户选择的数据源文件名

    // 获取数据列索引
    int getXColumn() const;
    int getYColumn() const;

    // 获取标签（默认为列名）
    QString getXLabel() const;
    QString getYLabel() const;

    // --- 样式获取 ---
    QCPScatterStyle::ScatterShape getPointShape() const;
    QColor getPointColor() const; // 获取点颜色下拉框的值

    Qt::PenStyle getLineStyle() const;
    QColor getLineColor() const;  // 获取线颜色下拉框的值
    int getLineWidth() const;

    // 是否在新窗口显示
    bool isNewWindow() const;

private slots:
    // 数据源文件改变时触发
    void onFileChanged(int index);

private:
    Ui::PlottingDialog1 *ui;

    // 存储所有可用模型
    QMap<QString, QStandardItemModel*> m_dataMap;
    // 当前选中的模型指针
    QStandardItemModel* m_currentModel;

    static int s_curveCounter; // 静态计数器，用于生成默认名称

    // 辅助函数
    void populateComboBoxes(); // 根据当前模型初始化数据列下拉框
    void setupStyleUI();       // 初始化样式控件（填充下拉框内容）
    void initColorComboBox(QComboBox* combo); // 通用函数：填充颜色下拉框

    // [新增] 图标生成辅助函数
    QIcon createPointIcon(QCPScatterStyle::ScatterShape shape);
    QIcon createLineIcon(Qt::PenStyle style);
};

#endif // PLOTTINGDIALOG1_H
