#include "processmanagement.h"
#include "ui_processmanagement.h"
#include <QDebug>
#include <qmessagebox.h>
#include<QTimer>
#include<QLabel>

//time的信号与槽

ProcessManagement::ProcessManagement(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProcessManagement)
{
    ui->setupUi(this);
    //初始化storage的长度
    for(int i=0;i<800;i++){
        storageNull[i] = true;
    }
    //绑定回车登录
    ui->Creat->setShortcut(Qt::Key_Enter);
}

ProcessManagement::~ProcessManagement()
{
    delete ui;
}

void ProcessManagement::on_Creat_clicked()
{
    //单击按钮后创建几个新的进程，并进行排序。
    ProcessNumber++;
    ReadyNumber++;
    if(ProcessNumber<=5){
        ProcessRight = true;
        QString ID = ui->NewID->text();
        QString Priority = ui->NewPriority->text();
        QString Time = ui->NewTime->text();
        QString Size = ui->NewSize->text();
        if(ID==nullptr||Priority==nullptr||Time==nullptr||Size==nullptr){
            ProcessRight = false;
        }

        if(ProcessRight){
            AddToStorage(ID,Priority,Time,Size);
            if(ProcessRight){
                int row_count = ui->InMemory->rowCount();
                int newRow = row_count+1;
                ui->InMemory->setRowCount(newRow);
                //注意这个地方newRow是表的高度，从1开始
                int Row = newRow-1;
                // ui->InMemory->item(MemoryRow,0)->setFlags(Qt::NoItemFlags);
                ui->InMemory->setItem(Row, 0, new QTableWidgetItem(ID));
                //  ui->InMemory->item(MemoryRow,1)->setFlags(Qt::NoItemFlags);
                ui->InMemory->setItem(Row, 1, new QTableWidgetItem(Priority));
                //  ui->InMemory->item(MemoryRow,2)->setFlags(Qt::NoItemFlags);
                ui->InMemory->setItem(Row, 2, new QTableWidgetItem(Time));
                //   ui->InMemory->item(MemoryRow,3)->setFlags(Qt::NoItemFlags);
                ui->InMemory->setItem(Row, 3, new QTableWidgetItem("Ready"));
                QTableWidgetItem *checkBox = new QTableWidgetItem();//添加多选框
                checkBox->setCheckState(Qt::Unchecked);
                ui->InMemory->setItem(Row, 4, checkBox);
            }
            else{
                //创建 QMessageBox 类对象
                ProcessNumber--;
                ReadyNumber--;
                QMessageBox MyBox(QMessageBox::Question,"Warning","内存不足，无法调度作业",QMessageBox::Yes|QMessageBox::No);
                //使 MyBox 对话框显示
                MyBox.exec();
            }
        }
        else{
            //创建 QMessageBox 类对象
            ProcessNumber--;
            ReadyNumber--;
            QMessageBox MyBox(QMessageBox::Question,"Warning","输入不能为空",QMessageBox::Yes|QMessageBox::No);
            //使 MyBox 对话框显示
            MyBox.exec();
        }
    }
    else{
        //创建 QMessageBox 类对象
        ProcessNumber--;
        ReadyNumber--;
        QMessageBox MyBox(QMessageBox::Question,"Warning","最多只能同时5个进程",QMessageBox::Yes|QMessageBox::No);
        //使 MyBox 对话框显示
        MyBox.exec();
    }
}

void ProcessManagement::AddToStorage(QString ID,QString Priority,QString Time,QString Size){

    bool fit = false;//是否有空位
    int TargetPlace = 0;//目标位置
    int length = 0; //空闲长度
    int NeedLength = Size.toInt();//需要的长度
    for(int i=0;i<800;i++){
        if(storageNull[i]){
            TargetPlace = i;//记录起始位置
            length = 0;//这里为0是因为下面的while中是第一个判断为ture的点
            while(storageNull[i]){
                length++;
                i++;
            }
            if(length>=NeedLength){
                fit = true;
                break;
            }
            else{
                TargetPlace = 0;//重置(其实没什么用)
                fit = false;
            }
        }
    }

    if(fit){
        QLabel *StorageIDName = new QLabel("ID:");
        QLabel *StorageID = new QLabel(ID);//先这么放着吧（指变量名）
        QLabel *StorageSizeName = new QLabel("Size:");
        QLabel *StorageSize = new QLabel(Size);
        StorageSize->setObjectName("Size");
        QLabel *TargetSpotName = new QLabel("TargetSpot:");
        QLabel *TargetSpot = new QLabel(QString::number(TargetPlace));
        TargetSpot->setObjectName("TargetSpot");

        QGridLayout *Process = new QGridLayout();
        Process->setRowStretch(0,1);
        Process->setRowStretch(1,2);
        Process->setColumnStretch(0,1);
        Process->setColumnStretch(1,1);
        Process->setSpacing(10);
        Process->addWidget(StorageIDName,0,0,1,1);
        Process->addWidget(StorageID,0,1,1,1);
        Process->addWidget(StorageSizeName,1,0,1,1);
        Process->addWidget(StorageSize,1,0,2,1);
        Process->addWidget(TargetSpotName,1,1,1,1);
        Process->addWidget(TargetSpot,1,1,2,1);

        QWidget *ProcessStorage = new QWidget(this);
        ProcessStorage->setObjectName(ID);//添加名字
        ProcessStorage->move(870+TargetPlace,664);
        ProcessStorage->resize(NeedLength,270);
        //将占用的长度定位false
        qDebug()<<"Need"<<NeedLength<<"    Target:"<<TargetPlace;
        for(int i=TargetPlace;i<NeedLength+TargetPlace;i++){
            storageNull[i] = false;
        }
        QFont font;
        font.setPixelSize(20);
        font.setFamily("simsun");
        ProcessStorage->setFont(font);
        QColor color(rand()%256,rand()%256,rand()%256);
        QString css  = QString("QWidget{background-color:%1;}").arg(color.name());
        ProcessStorage->setStyleSheet(css);
        ProcessStorage->setLayout(Process);
        ProcessStorage->show();
        //可以进入Ready队列
        ProcessRight=true;
    }
    else{
        ProcessRight=false;
    }
}

void ProcessManagement::PriorityReady(){
    //先写一个优先级排序
    List.clear();
    Process tempProcess;
    for(int i =0;i<ProcessNumber;i++){
        tempProcess.pid=ui->InMemory->item(i,0)->text();
        tempProcess.priority=ui->InMemory->item(i,1)->text().toInt();
        tempProcess.time=ui->InMemory->item(i,2)->text().toInt();
        tempProcess.state=ui->InMemory->item(i,3)->text();
        if(tempProcess.state=="Ready"){
            List.push_back(tempProcess);
        }
        else{
            ReadyNumber--;
        }
    }
    for(int i = 0;i<ReadyNumber;i++){
        for(int j =ReadyNumber- i-1;j>0;j--){
            if(List[j].priority<List[j-1].priority){
                Process p =List[j];
                List[j] = List[j-1];
                List[j-1] = p;
            }
        }
    }
    ui->Ready->clear();
    for(Process e:List)
    {
        QString id = e.pid;
        ui->Ready->addItem(id);
    }
    QString run = ui->RunID->text();
    bool running = false;
    if(run ==nullptr){
        running = true;
    }
    if(running){
        //下面这个地方写一个转到running状态的函数。
        PriorityRunning();
    }
    else{
        //do nothing
    }
}

void ProcessManagement::RoundRobinReady(){
    List.clear();
    ui->RunID->setText(nullptr);
    ui->RunTime->setText(nullptr);
    ui->RunPriority->setText(nullptr);
    Process tempProcess;
    for(int i =0;i<ProcessNumber;i++){
        tempProcess.pid=ui->InMemory->item(i,0)->text();
        tempProcess.priority=ui->InMemory->item(i,1)->text().toInt();
        tempProcess.time=ui->InMemory->item(i,2)->text().toInt();
        tempProcess.state=ui->InMemory->item(i,3)->text();
        tempProcess.Timepiece = tempProcess.time;
        if(tempProcess.state=="Ready"||tempProcess.state=="Running"){
            List.push_back(tempProcess);
        }
        else{
            ReadyNumber--;
        }
    }
    ui->Ready->clear();
    for(Process e:List)
    {
        QString id = e.pid;
        ui->Ready->addItem(id);
    }
    QString run = ui->RunID->text();
    bool running = false;
    if(run ==nullptr){
        running = true;
    }
    if(running){
        //下面这个地方写一个转到running状态的函数。
        RoundRobinRunning();
    }
    else{
        //do nothing
    }
}

void ProcessManagement::PriorityRunning(){
    if(List.length()>=1){
        RunID = List[0].pid;
        ui->RunID->setText(RunID);
        RunPriority = List[0].priority;
        ui->RunPriority->setText(QString::number(RunPriority));
        RunTime = List[0].time;
        ui->RunTime->setText(QString::number(RunTime));
        List[0].state = "Running";
        //我们还需要将InMemory中状态栏改一下
        ChangeState(List[0].pid,List[0].state);
        //把list第一个元素pop掉
        List.pop_front();
        //这个地方要刷新Ready队列
        ui->Ready->clear();
        for(Process e:List)
        {
            QString id = e.pid;
            qDebug()<<id;
            ui->Ready->addItem(id);
        }
        //   下面是设置时间
        BarTime = RunTime;
        TotalTime = RunTime;
        Timer = new QTimer();
        Timer->start(1000);
        connect(Timer, SIGNAL(timeout()), this, SLOT(ProcessRun()));
    }
    else {
        //停止时间
        Timer->stop();
        ui->RunningBar->setValue(0);
        QMessageBox MyBox(QMessageBox::Question,"Warning","运行已结束",QMessageBox::Yes|QMessageBox::No);
        //使 MyBox 对话框显示
        MyBox.exec();
    }
}

void ProcessManagement::RoundRobinRunning(){
    Timer->start(1000);
    if(List.length()>=1){
        List.back().state = "Ready";
        RunID = List[0].pid;
        ui->RunID->setText(RunID);
        RunPriority = List[0].priority;
        ui->RunPriority->setText(QString::number(RunPriority));
        RunTime = List[0].time;
        ui->RunTime->setText(QString::number(RunTime));
        List[0].state = "Running";
        //我们还需要将InMemory中状态栏改一下
        ChangeState(List[0].pid,List[0].state);
        //把list第一个元素pop掉
        Process tempProcess = List[0];
        List.pop_front();
        //这个地方要刷新Ready队列
        ui->Ready->clear();
        for(Process e:List)
        {
            QString id = e.pid;
            ui->Ready->addItem(id);
        }
        //重新放在ready尾部
        List.push_back(tempProcess);
        //   下面是设置时间
        BarTime = RunTime;
        TotalTime = RunTime;
    }
    else {
        //停止时间
        Timer->stop();
        ui->RunningBar->setValue(0);
        QMessageBox MyBox(QMessageBox::Question,"Warning","运行已结束",QMessageBox::Yes|QMessageBox::No);
        //使 MyBox 对话框显示
        MyBox.exec();
    }
}

void ProcessManagement::ProcessRun()
{
    for(int i =0;i<ProcessNumber;i++){
        QString targetID = ui->InMemory->item(i,0)->text();
        if(targetID == RunID){
            //修改时间
            ui->InMemory->item(i,2)->setText(QString::number(RunTime));
        }
    }
    if(RunTime>0){
        RunTime--;
        //静度条变化
        ui->RunningBar->setRange(0,TotalTime);
        ui->RunningBar->setValue(BarTime-RunTime);
        ui->RunTime->setText(QString::number(RunTime));
    }
    else{
        //停止时间
        Timer->stop();
        //这个地方还要将数据存入历史数据中
        AddRecord(RunID,RunPriority,RunTime);
        //删除label中的running数据
        ui->RunID->setText(nullptr);
        ui->RunPriority->setText(nullptr);
        ui->RunTime->setText(nullptr);
        PriorityRunning();
    }
}

void ProcessManagement::ProcessRun2()
{
    qDebug()<<"1";
    for(int i =0;i<ProcessNumber;i++){
        QString targetID = ui->InMemory->item(i,0)->text();
        if(targetID == RunID){
            //修改时间和状态
            ui->InMemory->item(i,2)->setText(QString::number(RunTime));
            ui->InMemory->item(i,3)->setText("Ready");
        }
    }
    if(RunTime>0){
        Timer->stop();
        RunTime--;
        //修改最后一个的time
        //       Process temptProcess = List.back();
        List.back().time--;
        //静度条变化
        ui->RunningBar->setRange(0,TotalTime);
        ui->RunningBar->setValue(BarTime-RunTime);
        ui->RunTime->setText(QString::number(RunTime));
        //这个地方不可以这样设计
        RoundRobinRunning();
    }
    else{
        List.pop_back();
        //停止时间
        Timer->stop();
        //这个地方还要将数据存入历史数据中
        AddRecord(RunID,RunPriority,RunTime);
        //删除label中的running数据
        ui->RunID->setText(nullptr);
        ui->RunPriority->setText(nullptr);
        ui->RunTime->setText(nullptr);
        RoundRobinRunning();
    }
}

void ProcessManagement::AddRecord(QString id,int priority,int time){
    QWidget *Finish= this->findChild<QWidget *>(id);
    QString TargetSpot = Finish->findChild<QLabel *>("TargetSpot")->text();
    int start = TargetSpot.toInt();
    qDebug()<<TargetSpot;
    QString Size = Finish->findChild<QLabel *>("Size")->text();
    int length = Size.toInt();
    qDebug()<<TargetSpot;
    //将内存置空
    for(int i=start;i<start+length;i++){
            storageNull[i]=true;
    }
    Finish->close();
    int row_count = ui->Record->rowCount();
    int newRow = row_count+1;
    ui->Record->setRowCount(newRow);
    //注意这个地方newRow是表的高度，从1开始
    int Row = newRow-1;
    ui->Record->setItem(Row,0,new QTableWidgetItem(id));
    ui->Record->setItem(Row,1,new QTableWidgetItem(QString::number(priority)));
    ui->Record->setItem(Row,2,new QTableWidgetItem(QString::number(time)));
    //这里还要找到InMemory中一行并进行删除
    for(int i =0;i<ProcessNumber;i++){
        QString targetID = ui->InMemory->item(i,0)->text();
        if(targetID == id){
            //删除这一行
            ui->InMemory->removeRow(i);
            ProcessNumber--;
            ReadyNumber--;
        }
    }
}

void ProcessManagement::ChangeState(QString id,QString state){
    for(int i =0;i<ProcessNumber;i++){
        QString targetID = ui->InMemory->item(i,0)->text();
        if(targetID == id){
            ui->InMemory->item(i,3)->setText(state);
        }
    }
}

void ProcessManagement::on_Pause_clicked()
{
    if(HangClicked){
        HangClicked = false;
        Timer->start(1000);
    }
    else{
        HangClicked = true;
        Timer->stop();
    }
}

void ProcessManagement::on_InMemory_itemClicked(QTableWidgetItem *item)
{
    for(int i =0; i<ProcessNumber;i++)
    {
        QString HangID = ui->InMemory->item(i,0)->text();
        QString HangState = ui->InMemory->item(i,3)->text();
        //改变进程状态
        bool Hang =ui->InMemory->item(i,4)->checkState();
        bool InHang = false;
        int HangHeight = ui->Hang->count();
        //这个地方我才用的是遍历Hang的list,其实还有一种方法，就是直接检测表中的状态是不是Hang
        for(int j=0;j<HangHeight;j++){
            if(ui->Hang->item(j)->text()==HangID){
                InHang = true;
            }
        }
        //如果被选中但是没在Hang队列中
        if(Hang){
            if(!InHang){
                if(HangState=="Running"){
                    ui->RunID->setText(nullptr);
                    ui->RunPriority->setText(nullptr);
                    ui->RunTime->setText(nullptr);
                    Timer->stop();
                    PriorityRunning();
                }
                ui->InMemory->item(i,3)->setText("Hang");
                ui->Hang->addItem(HangID);
            }
            else{
                //do nothing
            }
        }
        //如果没被选中但是在Hang队列中
        if(InHang){
            if(!Hang){
                for(int j=0;j<HangHeight;j++){
                    if(ui->Hang->item(j)->text()==HangID){
                        //重置为Ready
                        ui->InMemory->item(i,3)->setText("Ready");
                        ui->Hang->takeItem(j);
                        //
                    }
                }
            }
            else{
                //do nothing
            }
        }
    }
    //最后调用Ready，重置队列
    if(priorityReady){
        PriorityReady();
    }
    else if(roundRobinReady){
        RoundRobinReady();
    }
    else{
        //do nothing
    }
}

void ProcessManagement::on_ByPriority_clicked()
{
    priorityReady = true;
    PriorityReady();
}

void ProcessManagement::on_ByRoundRobin_clicked()
{
    roundRobinReady = true;
    Timer = new QTimer();
    RoundRobinReady();
    connect(Timer, SIGNAL(timeout()), this, SLOT(ProcessRun2()));
}

void ProcessManagement::on_WaitingButton_clicked()
{
    if(WaitClicked){
        WaitClicked = false;
        for(int i =0;i<ProcessNumber;i++){
            QString targetID = ui->InMemory->item(i,0)->text();
            if(targetID == WaitingID){
                ui->InMemory->item(i,3)->setText("Running");
            }
        }
        ui->Waiting->clear();
        ui->RunID->setText(WaitingID);
        ui->RunPriority->setText(WaitingPriority);
        ui->RunTime->setText(WaitingTime);
        Timer->start(1000);
    }
    else if(ui->RunID->text()!=nullptr){
        Timer->stop();
        WaitingID = ui->RunID->text();
        WaitingTime = ui->RunTime->text();
        WaitingPriority = ui->RunPriority->text();
        for(int i =0;i<ProcessNumber;i++){
            QString targetID = ui->InMemory->item(i,0)->text();
            if(targetID == WaitingID){
                ui->InMemory->item(i,3)->setText("Waiting");
                ui->InMemory->item(i,2)->setText(WaitingTime);
            }
        }
        ui->RunID->setText(nullptr);
        ui->RunPriority->setText(nullptr);
        ui->RunTime->setText(nullptr);
        ui->Waiting->addItem(WaitingID);
        WaitClicked = true;
    }
}
