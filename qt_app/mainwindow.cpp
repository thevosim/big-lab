#include "mainwindow.h"
#include "poly_parser.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <sstream>
#include <cctype>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) 
{
    setupUI();
    storageTypeCombo->setCurrentIndex(5);
    spinX->setLocale(QLocale::C);
    spinY->setLocale(QLocale::C);
    spinZ->setLocale(QLocale::C);
}

void MainWindow::setupUI() 
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Тип хранилища:"));
    storageTypeCombo = new QComboBox();
    storageTypeCombo->addItems({
        "Поисковое дерево (AVL)",
        "Поисковое дерево (RB)",
        "Хеш-таблица (цепочки)",
        "Хеш-таблица (открытая адресация)",
        "Таблица на массиве (неупорядоченная)",
        "Таблица на массиве (упорядоченная)"
    });
    topLayout->addWidget(storageTypeCombo);
    mainLayout->addLayout(topLayout);

    dataTable = new QTableWidget(0, 2);
    dataTable->setHorizontalHeaderLabels({"Ключ (Имя)", "Полином"});
    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(dataTable);

    QGridLayout* crudLayout = new QGridLayout();
    keyInput = new QLineEdit();
    keyInput->setPlaceholderText("Имя (например, a)");
    polyInput = new QLineEdit();
    polyInput->setPlaceholderText("Полином (например, 2*x^2*y - 3*z)");
    
    addBtn = new QPushButton("Добавить / Обновить");
    deleteBtn = new QPushButton("Удалить");
    findBtn = new QPushButton("Найти");

    crudLayout->addWidget(new QLabel("Ключ:"), 0, 0);
    crudLayout->addWidget(keyInput, 0, 1);
    crudLayout->addWidget(new QLabel("Полином:"), 1, 0);
    crudLayout->addWidget(polyInput, 1, 1);
    
    QHBoxLayout* crudBtns = new QHBoxLayout();
    crudBtns->addWidget(addBtn);
    crudBtns->addWidget(deleteBtn);
    crudBtns->addWidget(findBtn);
    crudLayout->addLayout(crudBtns, 2, 0, 1, 2);
    mainLayout->addLayout(crudLayout);

    QVBoxLayout* calcLayout = new QVBoxLayout();
    calcLayout->addWidget(new QLabel("Арифметические операции (например: a*b + c):"));
    exprInput = new QLineEdit();
    calcLayout->addWidget(exprInput);
    
    QHBoxLayout* exprResLayout = new QHBoxLayout();
    resultPolyOutput = new QLineEdit();
    resultPolyOutput->setReadOnly(true);
    calcExprBtn = new QPushButton("Вычислить выражение");
    exprResLayout->addWidget(calcExprBtn);
    exprResLayout->addWidget(resultPolyOutput);
    calcLayout->addLayout(exprResLayout);
    mainLayout->addLayout(calcLayout);

    QHBoxLayout* pointLayout = new QHBoxLayout();
    spinX = new QDoubleSpinBox(); spinX->setRange(-1000, 1000);
    spinY = new QDoubleSpinBox(); spinY->setRange(-1000, 1000);
    spinZ = new QDoubleSpinBox(); spinZ->setRange(-1000, 1000);
    
    pointLayout->addWidget(new QLabel("X:")); pointLayout->addWidget(spinX);
    pointLayout->addWidget(new QLabel("Y:")); pointLayout->addWidget(spinY);
    pointLayout->addWidget(new QLabel("Z:")); pointLayout->addWidget(spinZ);
    
    evalPointBtn = new QPushButton("Посчитать в точке");

    pointResultEdit = new QLineEdit();
    pointResultEdit->setReadOnly(true);
    pointResultEdit->setPlaceholderText("Результат");
    
    pointLayout->addWidget(evalPointBtn);
    pointLayout->addWidget(pointResultEdit);
    // -----------------------
    
    mainLayout->addLayout(pointLayout);

    connect(storageTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStorageTypeChanged);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(findBtn, &QPushButton::clicked, this, &MainWindow::onFindClicked);
    connect(calcExprBtn, &QPushButton::clicked, this, &MainWindow::onCalculateExprClicked);
    connect(evalPointBtn, &QPushButton::clicked, this, &MainWindow::onEvaluatePointClicked);
    connect(dataTable, &QTableWidget::itemSelectionChanged, this, [this]() {
    int row = dataTable->currentRow();
    if (row >= 0) {
        std::string key = dataTable->item(row, 0)->text().toStdString();
        const Polynomus* p = manager.getTable()->find(key);
        if (p) {
            // Синхронизируем внутреннее состояние с выбранной строкой
            lastCalculatedPoly = *p; 
            
            polyInput->setText(QString::fromStdString(polyToString(*p)));
            keyInput->setText(QString::fromStdString(key));
        }
    }
});
}

void MainWindow::updateTable() 
{
    dataTable->setRowCount(0);
    std::vector<std::pair<std::string, Polynomus>> items;
    manager.getTable()->collectAll(items);
    
    for (const auto& item : items) 
    {
        int row = dataTable->rowCount();
        dataTable->insertRow(row);
        dataTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(item.first)));
        dataTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(polyToString(item.second))));
    }
}

std::string MainWindow::polyToString(const Polynomus& p) 
{
    std::stringstream ss;
    ss << p;
    return ss.str();
}

void MainWindow::onStorageTypeChanged(int index) 
{
    manager.setTableType(static_cast<TableType>(index));
    updateTable();
}

void MainWindow::onAddClicked() 
{
    QString key = keyInput->text().trimmed();
    std::string expr = polyInput->text().toStdString();
    if (key.isEmpty() || expr.empty()) return;

    try 
    {
        Polynomus p = PolyParser::parse(expr);
        manager.getTable()->insert(key.toStdString(), p);
        updateTable();
        keyInput->clear();
        polyInput->clear();
    } 
    catch (const std::exception& e) 
    {
        QMessageBox::critical(this, "Ошибка парсинга", e.what());
    }
}

void MainWindow::onDeleteClicked() 
{
    QString key = keyInput->text().trimmed();
    if (key.isEmpty()) return;
    
    if (manager.getTable()->erase(key.toStdString())) 
    {
        updateTable();
        keyInput->clear();
    } 
    else 
    {
        QMessageBox::information(this, "Удаление", "Элемент с таким ключом не найден");
    }
}

void MainWindow::onFindClicked() 
{
    QString key = keyInput->text().trimmed();
    if (key.isEmpty()) return;
    
    const Polynomus* p = manager.getTable()->find(key.toStdString());
    if (p) 
    {
        lastCalculatedPoly = *p;
        polyInput->setText(QString::fromStdString(polyToString(*p)));
    }
    else 
    {
        QMessageBox::information(this, "Поиск", "Элемент не найден");
    }
}

Polynomus MainWindow::parseComplexExpression(const std::string& expr, bool& ok) 
{
    ok = true;
    std::string modifiedExpr = "";
    std::string token = "";

    auto flushToken = [&]() 
    {
        if (!token.empty()) 
        {
            if (token != "x" && token != "y" && token != "z" && std::isalpha(token[0])) 
            {
                const Polynomus* found = manager.getTable()->find(token);
                if (found) 
                {
                    // Добавляем спасительный ноль для обхода унарного минуса
                    std::string polyStr = polyToString(*found);
                    if (!polyStr.empty() && polyStr[0] != '-') {
                        polyStr = "+" + polyStr;
                    }
                    modifiedExpr += "(0" + polyStr + ")";
                } 
                else 
                {
                    throw std::runtime_error("Неизвестная переменная или ключ: " + token);
                }
            } 
            else 
            {
                modifiedExpr += token;
            }
            token.clear();
        }
    };

    for (char ch : expr) 
    {
        if (std::isalnum(ch)) 
        {
            token += ch;
        } 
        else 
        {
            flushToken();
            modifiedExpr += ch;
        }
    }
    flushToken();

    try 
    {
        return PolyParser::parse(modifiedExpr);
    } 
    catch (const std::exception& e) 
    {
        ok = false;
        throw;
    }
}

void MainWindow::onCalculateExprClicked() 
{
    std::string expr = exprInput->text().toStdString();
    if (expr.empty()) return;

    size_t eqPos = expr.find('=');
    QString targetKey = "";
    if (eqPos != std::string::npos) 
    {
        std::string keyStr = expr.substr(0, eqPos);
        keyStr.erase(std::remove_if(keyStr.begin(), keyStr.end(), ::isspace), keyStr.end());
        targetKey = QString::fromStdString(keyStr);
        expr = expr.substr(eqPos + 1);
    }

    bool ok;
    try 
    {
        lastCalculatedPoly = parseComplexExpression(expr, ok);
        resultPolyOutput->setText(QString::fromStdString(polyToString(lastCalculatedPoly)));
        
        if (!targetKey.isEmpty()) 
        {
            manager.getTable()->insert(targetKey.toStdString(), lastCalculatedPoly);
            updateTable();
        }
    } 
    catch (const std::exception& e) 
    {
        QMessageBox::critical(this, "Ошибка вычисления", e.what());
    }
}

void MainWindow::onEvaluatePointClicked() 
{
    spinX->clearFocus(); spinY->clearFocus(); spinZ->clearFocus();

    double x = spinX->value();
    double y = spinY->value();
    double z = spinZ->value();
    
    double res = lastCalculatedPoly.evaluate(x, y, z);
    pointResultEdit->setText(QString::number(res));
}