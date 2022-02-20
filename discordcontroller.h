#ifndef DISCORDCONTROLLER_H
#define DISCORDCONTROLLER_H

#include <QObject>
#include <QThread>

#include "discord-files/discord.h"

const QString settingPath = "config.ini";

struct StateSetting{
    discord::ClientId clientID;
    quint32 updatePeriod;
    QString details;
    QString state;
    QString smallImage;
    QString smallTooltip;
    QString largeImage;
    QString largeTooltip;
    discord::Timestamp startTimeStamp;
    discord::Timestamp endTimeStamp;
};

StateSetting ReadSetting();
void ChangeSetting(StateSetting);

struct DiscordState{
    discord::User currentUser;
    std::unique_ptr<discord::Core> core;
};

class DiscordController: public QThread
{
    Q_OBJECT
public:
    DiscordController(QObject *parent = nullptr);
    void run() override;


private:
    qint64 startTime;
    StateSetting stateSetting;

    void StartCustomState();
    void UpdateState(DiscordState *state, uint sleepMillSec);

};

#endif // DISCORDCONTROLLER_H
