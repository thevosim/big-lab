#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <memory>
#include "poly_manager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onStorageTypeChanged(int index);
    void onAddClicked();
    void onDeleteClicked();
    void onFindClicked();
    void onCalculateExprClicked();
    void onEvaluatePointClicked();

private:
    void setupUI();
    void updateTable();
    std::string polyToString(const Polynomus& p);
    Polynomus parseComplexExpression(const std::string& expr, bool& ok);

    PolyManager manager;
    Polynomus lastCalculatedPoly;

    QComboBox* storageTypeCombo;
    QTableWidget* dataTable;
    
    QLineEdit* keyInput;
    QLineEdit* polyInput;
    QPushButton* addBtn;
    QPushButton* deleteBtn;
    QPushButton* findBtn;

    QLineEdit* exprInput;
    QLineEdit* resultPolyOutput;
    QPushButton* calcExprBtn;

    QDoubleSpinBox* spinX;
    QDoubleSpinBox* spinY;
    QDoubleSpinBox* spinZ;
    QLineEdit* pointResultEdit;
    QPushButton* evalPointBtn;
};