/*
 * 文件名: plottingdialog2.cpp
 * 文件作用: 压力-产量双坐标曲线配置对话框实现
 * 功能描述:
 * 1. 实现双文件数据源的选择与联动。
 * 2. 产量部分支持根据“绘图类型”动态显示/隐藏样式设置控件。
 * 3. 样式UI完全中文化、图表化。
 * 4. [新增] 获取“在新建窗口显示”勾选框状态。
 */

#include "plottingdialog2.h"
#include "ui_plottingdialog2.h"
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>

int PlottingDialog2::s_chartCounter = 1;

PlottingDialog2::PlottingDialog2(const QMap<QString, QStandardItemModel*>& models, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlottingDialog2),
    m_dataMap(models),
    m_pressModel(nullptr),
    m_prodModel(nullptr)
{
    ui->setupUi(this);

    // 1. 初始化样式UI (下拉框内容填充)
    setupStyleUI();

    // 2. 设置默认名称
    ui->lineEdit_Name->setText(QString("双坐标图表 %1").arg(s_chartCounter++));

    // 3. 汉化按钮
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    // 4. 初始化文件下拉框
    ui->combo_PressFile->clear();
    ui->combo_ProdFile->clear();

    if (!m_dataMap.isEmpty()) {
        for(auto it = m_dataMap.begin(); it != m_dataMap.end(); ++it) {
            QString filePath = it.key();
            QFileInfo fi(filePath);
            QString name = fi.fileName().isEmpty() ? filePath : fi.fileName();
            ui->combo_PressFile->addItem(name, filePath);
            ui->combo_ProdFile->addItem(name, filePath);
        }
    } else {
        ui->combo_PressFile->setEnabled(false);
        ui->combo_ProdFile->setEnabled(false);
    }

    // 5. 信号连接
    connect(ui->combo_PressFile, SIGNAL(currentIndexChanged(int)), this, SLOT(onPressFileChanged(int)));
    connect(ui->combo_ProdFile, SIGNAL(currentIndexChanged(int)), this, SLOT(onProdFileChanged(int)));
    connect(ui->combo_ProdType, SIGNAL(currentIndexChanged(int)), this, SLOT(onProdTypeChanged(int)));

    // 6. 初始状态触发
    if (ui->combo_PressFile->count() > 0) {
        ui->combo_PressFile->setCurrentIndex(0);
        onPressFileChanged(0);
    }
    if (ui->combo_ProdFile->count() > 0) {
        ui->combo_ProdFile->setCurrentIndex(0);
        onProdFileChanged(0);
    }

    // 默认触发一次类型切换以设置初始显隐
    onProdTypeChanged(ui->combo_ProdType->currentIndex());
}

PlottingDialog2::~PlottingDialog2()
{
    delete ui;
}

// --- 文件与列处理 ---

void PlottingDialog2::onPressFileChanged(int index) {
    Q_UNUSED(index);
    QString key = ui->combo_PressFile->currentData().toString();
    m_pressModel = m_dataMap.value(key, nullptr);
    populatePressColumns();
}

void PlottingDialog2::onProdFileChanged(int index) {
    Q_UNUSED(index);
    QString key = ui->combo_ProdFile->currentData().toString();
    m_prodModel = m_dataMap.value(key, nullptr);
    populateProdColumns();
}

void PlottingDialog2::populatePressColumns() {
    ui->combo_PressX->clear();
    ui->combo_PressY->clear();
    if (!m_pressModel) return;
    QStringList headers;
    for(int i=0; i<m_pressModel->columnCount(); ++i) {
        QStandardItem* item = m_pressModel->horizontalHeaderItem(i);
        headers << (item ? item->text() : QString("列 %1").arg(i+1));
    }
    ui->combo_PressX->addItems(headers);
    ui->combo_PressY->addItems(headers);
    if(headers.count() > 0) ui->combo_PressX->setCurrentIndex(0);
    if(headers.count() > 1) ui->combo_PressY->setCurrentIndex(1);
}

void PlottingDialog2::populateProdColumns() {
    ui->combo_ProdX->clear();
    ui->combo_ProdY->clear();
    if (!m_prodModel) return;
    QStringList headers;
    for(int i=0; i<m_prodModel->columnCount(); ++i) {
        QStandardItem* item = m_prodModel->horizontalHeaderItem(i);
        headers << (item ? item->text() : QString("列 %1").arg(i+1));
    }
    ui->combo_ProdX->addItems(headers);
    ui->combo_ProdY->addItems(headers);
    if(headers.count() > 0) ui->combo_ProdX->setCurrentIndex(0);
    if(headers.count() > 1) ui->combo_ProdY->setCurrentIndex(1);
}

// --- 样式逻辑 ---

void PlottingDialog2::onProdTypeChanged(int index)
{
    // index: 0=阶梯图, 1=折线图, 2=散点图
    bool showPointSettings = (index == 2); // 只有散点图显示点设置
    bool showLineSettings = true;          // 所有类型都允许设置线(阶梯线/折线/散点连线)

    ui->label_ProdPointShape->setVisible(showPointSettings);
    ui->combo_ProdPointShape->setVisible(showPointSettings);
    ui->label_ProdPointColor->setVisible(showPointSettings);
    ui->combo_ProdPointColor->setVisible(showPointSettings);

    ui->label_ProdLineStyle->setVisible(showLineSettings);
    ui->combo_ProdLineStyle->setVisible(showLineSettings);
    ui->label_ProdLineColor->setVisible(showLineSettings);
    ui->combo_ProdLineColor->setVisible(showLineSettings);
    ui->label_ProdLineWidth->setVisible(showLineSettings);
    ui->spin_ProdLineWidth->setVisible(showLineSettings);
}

void PlottingDialog2::setupStyleUI()
{
    // 1. 绘图类型
    ui->combo_ProdType->clear();
    ui->combo_ProdType->addItem("阶梯图", 0);
    ui->combo_ProdType->addItem("折线图", 1);
    ui->combo_ProdType->addItem("散点图", 2);

    // 2. 准备列表
    QList<QComboBox*> shapeCombos = {ui->combo_PressPointShape, ui->combo_ProdPointShape};
    QList<QComboBox*> lineCombos = {ui->combo_PressLineStyle, ui->combo_ProdLineStyle};
    QList<QComboBox*> colorCombos = {
        ui->combo_PressPointColor, ui->combo_PressLineColor,
        ui->combo_ProdPointColor, ui->combo_ProdLineColor
    };

    // 3. 填充点形状
    for(auto cb : shapeCombos) {
        cb->clear();
        cb->setIconSize(QSize(16, 16));
        cb->addItem(createPointIcon(QCPScatterStyle::ssDisc), "实心圆", (int)QCPScatterStyle::ssDisc);
        cb->addItem(createPointIcon(QCPScatterStyle::ssCircle), "空心圆", (int)QCPScatterStyle::ssCircle);
        cb->addItem(createPointIcon(QCPScatterStyle::ssSquare), "正方形", (int)QCPScatterStyle::ssSquare);
        cb->addItem(createPointIcon(QCPScatterStyle::ssDiamond), "菱形", (int)QCPScatterStyle::ssDiamond);
        cb->addItem(createPointIcon(QCPScatterStyle::ssTriangle), "三角形", (int)QCPScatterStyle::ssTriangle);
        cb->addItem(createPointIcon(QCPScatterStyle::ssCross), "十字", (int)QCPScatterStyle::ssCross);
        cb->addItem(createPointIcon(QCPScatterStyle::ssPlus), "加号", (int)QCPScatterStyle::ssPlus);
        cb->addItem(createPointIcon(QCPScatterStyle::ssNone), "无", (int)QCPScatterStyle::ssNone);
    }

    // 4. 填充线型
    for(auto cb : lineCombos) {
        cb->clear();
        cb->setIconSize(QSize(32, 16));
        cb->addItem(createLineIcon(Qt::NoPen), "无", (int)Qt::NoPen);
        cb->addItem(createLineIcon(Qt::SolidLine), "实线", (int)Qt::SolidLine);
        cb->addItem(createLineIcon(Qt::DashLine), "虚线", (int)Qt::DashLine);
        cb->addItem(createLineIcon(Qt::DotLine), "点线", (int)Qt::DotLine);
        cb->addItem(createLineIcon(Qt::DashDotLine), "点划线", (int)Qt::DashDotLine);
    }

    // 5. 填充颜色
    for(auto cb : colorCombos) {
        initColorComboBox(cb);
    }

    // 6. 设置默认值
    // 压力: 红色实心点，无连线
    int redIdx = ui->combo_PressPointColor->findData(QColor(Qt::red));
    if(redIdx != -1) ui->combo_PressPointColor->setCurrentIndex(redIdx);
    ui->combo_PressPointShape->setCurrentIndex(0); // 实心圆
    ui->combo_PressLineStyle->setCurrentIndex(0);  // 无

    // 产量: 蓝色，阶梯图，无点，蓝色实线(默认类型为阶梯图时，点设置隐藏，但这里预设好)
    int blueIdx = ui->combo_ProdLineColor->findData(QColor(Qt::blue));
    if(blueIdx != -1) ui->combo_ProdLineColor->setCurrentIndex(blueIdx);
    // 线型默认实线(因为阶梯图需要线)
    int solidIdx = ui->combo_ProdLineStyle->findData((int)Qt::SolidLine);
    if(solidIdx != -1) ui->combo_ProdLineStyle->setCurrentIndex(solidIdx);

    ui->spin_PressLineWidth->setValue(2);
    ui->spin_ProdLineWidth->setValue(2);
}

void PlottingDialog2::initColorComboBox(QComboBox* combo) {
    combo->clear();
    combo->setIconSize(QSize(16, 16));
    struct ColorItem { QString name; QColor color; };
    QList<ColorItem> colors = {
        {"黑色", Qt::black}, {"红色", Qt::red}, {"蓝色", Qt::blue},
        {"绿色", Qt::green}, {"青色", Qt::cyan}, {"品红", Qt::magenta},
        {"黄色", Qt::yellow}, {"深红", Qt::darkRed}, {"深绿", Qt::darkGreen},
        {"深蓝", Qt::darkBlue}, {"灰色", Qt::gray}, {"橙色", QColor(255, 165, 0)},
        {"紫色", QColor(128, 0, 128)}, {"棕色", QColor(165, 42, 42)},
        {"粉色", QColor(255, 192, 203)}, {"天蓝", QColor(135, 206, 235)}
    };
    for (const auto& item : colors) {
        QPixmap pix(16, 16);
        pix.fill(item.color);
        QPainter painter(&pix);
        painter.setPen(Qt::gray);
        painter.drawRect(0, 0, 15, 15);
        combo->addItem(QIcon(pix), item.name, item.color);
    }
}

QIcon PlottingDialog2::createPointIcon(QCPScatterStyle::ScatterShape shape) {
    QPixmap pix(16, 16);
    pix.fill(Qt::transparent);
    QCPPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QCPScatterStyle ss(shape);
    ss.setPen(QPen(Qt::black));
    ss.setBrush(QBrush(Qt::black));
    ss.setSize(10);
    ss.drawShape(&painter, 8, 8);
    return QIcon(pix);
}

QIcon PlottingDialog2::createLineIcon(Qt::PenStyle style) {
    QPixmap pix(32, 16);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    if (style == Qt::NoPen) {
        painter.setPen(Qt::gray);
        painter.drawText(pix.rect(), Qt::AlignCenter, "无");
    } else {
        QPen pen(Qt::black);
        pen.setStyle(style);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(0, 8, 32, 8);
    }
    return QIcon(pix);
}

// --- Getters ---

QString PlottingDialog2::getChartName() const { return ui->lineEdit_Name->text(); }

QString PlottingDialog2::getPressFileName() const { return ui->combo_PressFile->currentData().toString(); }
int PlottingDialog2::getPressXCol() const { return ui->combo_PressX->currentIndex(); }
int PlottingDialog2::getPressYCol() const { return ui->combo_PressY->currentIndex(); }
QString PlottingDialog2::getPressLegend() const { return ui->combo_PressY->currentText(); }

QCPScatterStyle::ScatterShape PlottingDialog2::getPressShape() const {
    return (QCPScatterStyle::ScatterShape)ui->combo_PressPointShape->currentData().toInt();
}
QColor PlottingDialog2::getPressPointColor() const {
    return ui->combo_PressPointColor->currentData().value<QColor>();
}
Qt::PenStyle PlottingDialog2::getPressLineStyle() const {
    return (Qt::PenStyle)ui->combo_PressLineStyle->currentData().toInt();
}
QColor PlottingDialog2::getPressLineColor() const {
    return ui->combo_PressLineColor->currentData().value<QColor>();
}
int PlottingDialog2::getPressLineWidth() const { return ui->spin_PressLineWidth->value(); }

QString PlottingDialog2::getProdFileName() const { return ui->combo_ProdFile->currentData().toString(); }
int PlottingDialog2::getProdXCol() const { return ui->combo_ProdX->currentIndex(); }
int PlottingDialog2::getProdYCol() const { return ui->combo_ProdY->currentIndex(); }
QString PlottingDialog2::getProdLegend() const { return ui->combo_ProdY->currentText(); }

int PlottingDialog2::getProdGraphType() const { return ui->combo_ProdType->currentData().toInt(); }

QCPScatterStyle::ScatterShape PlottingDialog2::getProdPointShape() const {
    return (QCPScatterStyle::ScatterShape)ui->combo_ProdPointShape->currentData().toInt();
}
QColor PlottingDialog2::getProdPointColor() const {
    return ui->combo_ProdPointColor->currentData().value<QColor>();
}
Qt::PenStyle PlottingDialog2::getProdLineStyle() const {
    return (Qt::PenStyle)ui->combo_ProdLineStyle->currentData().toInt();
}
QColor PlottingDialog2::getProdLineColor() const {
    return ui->combo_ProdLineColor->currentData().value<QColor>();
}
int PlottingDialog2::getProdLineWidth() const { return ui->spin_ProdLineWidth->value(); }

// [新增] 返回新建窗口勾选状态
bool PlottingDialog2::isNewWindow() const {
    return ui->check_NewWindow->isChecked();
}
