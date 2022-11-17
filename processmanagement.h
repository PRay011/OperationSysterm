#ifndef PROCESSMANAGEMENT_H
#define PROCESSMANAGEMENT_H

#include <QMainWindow>
#include <QTableWidgetItem>

namespace Ui {
class ProcessManagement;
}

class ProcessManagement : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProcessManagement(QWidget *parent = nullptr);
    ~ProcessManagement();

private slots:
    void on_Creat_clicked();
    void ProcessRun();
    void ProcessRun2();
    void PriorityReady();
    void RoundRobinReady();
    void PriorityRunning();
    void RoundRobinRunning();
    void ChangeState(QString,QString);
    void AddRecord(QString,int,int);
    void on_Pause_clicked();
    //void on_InMemory_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_InMemory_itemClicked(QTableWidgetItem *item);
    void on_ByPriority_clicked();
    void on_ByRoundRobin_clicked();
    void on_WaitingButton_clicked();
    void AddToStorage(QString,QString,QString,QString);

private:
    struct Process
    {
        QString pid;
        int priority;
        int time;
        QString state;
        int Timepiece;
    };

    Ui::ProcessManagement *ui;
    QList <Process>List;
    QTimer *Timer;
    int ProcessNumber = 0;
    int ReadyNumber = 0;
    int BarTime = 0;
    QString RunID;
    int RunTime = 0;
    int TotalTime = 0;
    int RunPriority;
    bool HangClicked = false;//这个用来记录次数
    QString WaitingID;
    QString WaitingTime;
    QString WaitingPriority;
    bool WaitClicked = false;
    bool priorityReady = false;
    bool roundRobinReady = false;
    bool storageNull[800];
    bool ProcessRight;

};

#endif // PROCESSMANAGEMENT_H
