#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "command-packet.pb.h"
#include "QDebug"
#include <string>
static char playOptions[][100] = {
    "TestPlay",
    "SetPosition",
    "PenaltyOur",
    "PenaltyOpp",
        "Stop",
     "OurGoalKick",
     "OppGoalKick",
     "SuperOffense",
     "Offense1",
     "Offense2",
     "SuperDefense",
     "Defense1",
     "Defense2",
     "PositionOurKickoff",
     "PositionOppKickoff",
      "Kickoff",
    "PositionOurPenalty",
     "TakeGoalKick",
     "PositionOurFreeKick",
     "PositionOppFreeKick",
     "PositionOurFreeBall",
     "PositionOppFreeBall",
     "PositionOppPenalty",
     "PositionOurGoalKick",
     "PositionOppGoalKick",
     "PositionGather",
     "OppFreeBall",
     "OurFreeBall",
     "OppFreeKick",
     "OurFreeKick",
     "None"
};
int playLen = sizeof(playOptions)/sizeof(char[100]);
static  char tacticOptions[][100] = {
    "Block",
    "ChargeBall",
    "CoverGoal",
    "DragToGoal",
    "DefendLine",
    "DefendPoint",
    "GoalieOur",
    "GoalieOpp",
    "MarkBot",
    "Pass",
    "Kickoff",
    "Position",
    "PositionForPenalty",
    "PositionForStart",
    "ReceiveBall",
    "Defend",
    "Attack",
    "Steal",
    "Stop",
    "Velocity",
    "Backup"
};
int tacticLen = sizeof(tacticOptions)/sizeof(char[100]);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    logger = new Logger("whatever.txt");
    std::string addr = "224.5.23.1";
    std::string interface;
    std::string port = "10001";
    publisher = new Publisher(*logger, addr, port, interface);
    for(int i=0; i<playLen; i++) {
        ui->comboBox->addItem(QString(playOptions[i]));
    }
    for(int i=0; i<tacticLen; i++) {
        ui->comboBox_2->addItem(QString(tacticOptions[i]));
    }
    for(int i=0; i<5; i++) {
        ui->comboBox_3->addItem(QString::number(i));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    StrategyPacket packet;
    packet.set_which(StrategyPacket::PLAY);
    int playid = ui->comboBox->currentIndex()+1;
    qDebug() << playid;
    packet.set_play(StrategyPacket::Play(playid));
    publisher->publish(packet);
    qDebug() << "Sent play packet";
}

void MainWindow::on_pushButton_2_clicked()
{
    StrategyPacket packet;
    packet.set_which(StrategyPacket::TACTIC);
    StrategyPacket::TacticID tid = StrategyPacket::TacticID(ui->comboBox_2->currentIndex()+1);
    int botid = ui->comboBox_3->currentIndex();
    StrategyPacket::Tactic t;
    t.set_botid(botid);
    t.set_tid(tid);
//    qDebug() << playid;
    *packet.mutable_tactic() = t;
    publisher->publish(packet);
    qDebug() << "Sent tactic packet";
}
