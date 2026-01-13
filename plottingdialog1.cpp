/*
 * 文件名: plottingdialog1.cpp
 * 文件作用: 新建单一曲线配置对话框实现
 * 功能描述:
 * 1. 实现数据列加载，支持多文件切换联动。
 * 2. [修改] 样式设置 UI 优化：
 * - 移除了下拉框中的英文描述。
 * - 为点形状和线类型增加了动态生成的预览图标。
 * - 颜色选择纯中文化。
 */

#include "plottingdialog1.h"
#include "ui_plottingdialog1.h"
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>

// 初始化静态计数器
int PlottingDialog1::s_curveCounter = 1;

PlottingDialog1::PlottingDialog1(const QMap<QString, QStandardItemModel*>& models, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlottingDialog1),
    m_dataMap(models),
    m_currentModel(nullptr)
{
    ui->setupUi(this);

    // 1. 初始化样式控件内容 (填充下拉框，生成图标)
    setupStyleUI();

    // 2. 汉化标准按钮
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    // 3. 设置默认名称
    ui->lineEdit_Name->setText(QString("曲线 %1").arg(s_curveCounter++));

    // 4. 初始化文件选择下拉框
    ui->comboFileSelect->clear();
    if (m_dataMap.isEmpty()) {
        ui->comboFileSelect->setEnabled(false);
    } else {
        for(auto it = m_dataMap.begin(); it != m_dataMap.end(); ++it) {
            QString filePath = it.key();
            QFileInfo fi(filePath);
            ui->comboFileSelect->addItem(fi.fileName().isEmpty() ? filePath : fi.fileName(), filePath);
        }
    }

    // 5. 信号连接
    connect(ui->comboFileSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(onFileChanged(int)));

    // 6. 触发初始加载
    if (ui->comboFileSelect->count() > 0) {
        ui->comboFileSelect->setCurrentIndex(0);
        onFileChanged(0);
    }
}

PlottingDialog1::~PlottingDialog1()
{
    delete ui;
}

void PlottingDialog1::onFileChanged(int index)
{
    Q_UNUSED(index);
    QString key = ui->comboFileSelect->currentData().toString();

    if (m_dataMap.contains(key)) {
        m_currentModel = m_dataMap.value(key);
    } else {
        m_currentModel = nullptr;
    }
    populateComboBoxes();
}

void PlottingDialog1::populateComboBoxes()
{
    ui->combo_XCol->clear();
    ui->combo_YCol->clear();

    if (!m_currentModel) return;

    QStringList headers;
    for(int i=0; i<m_currentModel->columnCount(); ++i) {
        QStandardItem* item = m_currentModel->horizontalHeaderItem(i);
        headers << (item ? item->text() : QString("列 %1").arg(i+1));
    }
    ui->combo_XCol->addItems(headers);
    ui->combo_YCol->addItems(headers);

    if(headers.count() > 0) ui->combo_XCol->setCurrentIndex(0);
    if(headers.count() > 1) ui->combo_YCol->setCurrentIndex(1);
}

void PlottingDialog1::setupStyleUI()
{
    // 1. 填充点形状 (图标 + 纯中文)
    ui->combo_PointShape->clear();
    ui->combo_PointShape->setIconSize(QSize(16, 16)); // 设置图标大小

    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssDisc), "实心圆", (int)QCPScatterStyle::ssDisc);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssCircle), "空心圆", (int)QCPScatterStyle::ssCircle);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssSquare), "正方形", (int)QCPScatterStyle::ssSquare);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssDiamond), "菱形", (int)QCPScatterStyle::ssDiamond);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssTriangle), "三角形", (int)QCPScatterStyle::ssTriangle);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssCross), "十字", (int)QCPScatterStyle::ssCross);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssPlus), "加号", (int)QCPScatterStyle::ssPlus);
    ui->combo_PointShape->addItem(createPointIcon(QCPScatterStyle::ssNone), "无", (int)QCPScatterStyle::ssNone);

    // 2. 填充线型 (宽图标 + 纯中文)
    ui->combo_LineStyle->clear();
    ui->combo_LineStyle->setIconSize(QSize(32, 16)); // 线条图标宽一些，展示虚线效果

    ui->combo_LineStyle->addItem(createLineIcon(Qt::NoPen), "无", (int)Qt::NoPen);
    ui->combo_LineStyle->addItem(createLineIcon(Qt::SolidLine), "实线", (int)Qt::SolidLine);
    ui->combo_LineStyle->addItem(createLineIcon(Qt::DashLine), "虚线", (int)Qt::DashLine);
    ui->combo_LineStyle->addItem(createLineIcon(Qt::DotLine), "点线", (int)Qt::DotLine);
    ui->combo_LineStyle->addItem(createLineIcon(Qt::DashDotLine), "点划线", (int)Qt::DashDotLine);

    // 3. 填充颜色 (图标 + 纯中文)
    initColorComboBox(ui->combo_PointColor);
    initColorComboBox(ui->combo_LineColor);

    // 4. 设置默认值
    // 点：默认红色，实心圆
    int redIdx = ui->combo_PointColor->findData(QColor(Qt::red));
    if (redIdx != -1) ui->combo_PointColor->setCurrentIndex(redIdx);
    ui->combo_PointShape->setCurrentIndex(0); // 实心圆

    // 线：默认蓝色，无 (Qt::NoPen)
    int blueIdx = ui->combo_LineColor->findData(QColor(Qt::blue));
    if (blueIdx != -1) ui->combo_LineColor->setCurrentIndex(blueIdx);
    ui->combo_LineStyle->setCurrentIndex(0); // 无

    // 线宽
    ui->spin_LineWidth->setValue(2);
}

void PlottingDialog1::initColorComboBox(QComboBox* combo)
{
    combo->clear();
    combo->setIconSize(QSize(16, 16));

    // 定义常用颜色列表 (纯中文)
    struct ColorItem { QString name; QColor color; };
    QList<ColorItem> colors = {
        {"黑色", Qt::black},
        {"红色", Qt::red},
        {"蓝色", Qt::blue},
        {"绿色", Qt::green},
        {"青色", Qt::cyan},
        {"品红", Qt::magenta},
        {"黄色", Qt::yellow},
        {"深红", Qt::darkRed},
        {"深绿", Qt::darkGreen},
        {"深蓝", Qt::darkBlue},
        {"灰色", Qt::gray},
        {"橙色", QColor(255, 165, 0)},
        {"紫色", QColor(128, 0, 128)},
        {"棕色", QColor(165, 42, 42)},
        {"粉色", QColor(255, 192, 203)},
        {"天蓝", QColor(135, 206, 235)}
    };

    // 生成带颜色块的图标
    for (const auto& item : colors) {
        QPixmap pix(16, 16);
        pix.fill(item.color);

        QPainter painter(&pix);
        painter.setPen(Qt::gray);
        painter.drawRect(0, 0, 15, 15); // 绘制边框

        combo->addItem(QIcon(pix), item.name, item.color);
    }
}

// [新增] 生成点形状图标
QIcon PlottingDialog1::createPointIcon(QCPScatterStyle::ScatterShape shape)
{
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);

    QCPPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);

    // 使用黑色绘制图标，清晰可见
    QCPScatterStyle ss(shape);
    ss.setPen(QPen(Qt::black));
    // 对于实心形状(Disc等)填充黑色，空心形状(Circle等)会自动忽略Brush
    ss.setBrush(QBrush(Qt::black));
    ss.setSize(10);

    ss.drawShape(&painter, 8, 8); // 绘制在中心 (8,8)

    return QIcon(pix);
}

// [新增] 生成线型图标
QIcon PlottingDialog1::createLineIcon(Qt::PenStyle style)
{
    QPixmap pix(32, 16); // 宽度32，适合展示虚线模式
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);

    if (style == Qt::NoPen) {
        // 如果是无，画一个灰色的叉或者文字
        painter.setPen(Qt::gray);
        painter.drawText(pix.rect(), Qt::AlignCenter, "无");
    } else {
        QPen pen(Qt::black);
        pen.setStyle(style);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(0, 8, 32, 8); // 绘制横线
    }

    return QIcon(pix);
}

// --- Getters ---

QString PlottingDialog1::getCurveName() const { return ui->lineEdit_Name->text(); }
QString PlottingDialog1::getLegendName() const { return ui->combo_YCol->currentText(); }
QString PlottingDialog1::getSelectedFileName() const { return ui->comboFileSelect->currentData().toString(); }

int PlottingDialog1::getXColumn() const { return ui->combo_XCol->currentIndex(); }
int PlottingDialog1::getYColumn() const { return ui->combo_YCol->currentIndex(); }
QString PlottingDialog1::getXLabel() const { return ui->combo_XCol->currentText(); }
QString PlottingDialog1::getYLabel() const { return ui->combo_YCol->currentText(); }
bool PlottingDialog1::isNewWindow() const { return ui->check_NewWindow->isChecked(); }

// 直接从控件获取值
QCPScatterStyle::ScatterShape PlottingDialog1::getPointShape() const {
    return (QCPScatterStyle::ScatterShape)ui->combo_PointShape->currentData().toInt();
}
QColor PlottingDialog1::getPointColor() const {
    return ui->combo_PointColor->currentData().value<QColor>();
}
Qt::PenStyle PlottingDialog1::getLineStyle() const {
    return (Qt::PenStyle)ui->combo_LineStyle->currentData().toInt();
}
QColor PlottingDialog1::getLineColor() const {
    return ui->combo_LineColor->currentData().value<QColor>();
}
int PlottingDialog1::getLineWidth() const {
    return ui->spin_LineWidth->value();
}
