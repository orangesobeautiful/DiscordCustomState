#include "discordcontroller.h"

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QApplication>
#include <QSettings>

#include <csignal>

#include "discord-files/discord.h"

namespace{
volatile bool interrupted{false};
}

// 讀取設定檔
StateSetting ReadSetting(){
    StateSetting res;
    QSettings setting(settingPath, QSettings::IniFormat);

    res.clientID = setting.value("ClientID").toLongLong();
    res.updatePeriod = setting.value("UpdatePeriod").toUInt();

    setting.beginGroup("State");
    res.details = setting.value("Details").toString();
    res.state = setting.value("State").toString();
    res.startTimeStamp = setting.value("StartTimestamp").toLongLong();
    res.endTimeStamp = setting.value("EndTimestamp").toLongLong();
    setting.endGroup();

    setting.beginGroup("Images");
    res.largeImage = setting.value("LargeImage").toString();
    res.largeTooltip = setting.value("LargeTooltip").toString();
    res.smallImage = setting.value("SmallImage").toString();
    res.smallTooltip = setting.value("SmallTooltip").toString();
    setting.endGroup();

    return res;
}

// 修改設定
void ChangeSetting(StateSetting stateSetting){
    QSettings iniSetting(settingPath, QSettings::IniFormat);
    iniSetting.setValue("ClientID", QString::number(stateSetting.clientID));
    iniSetting.setValue("UpdatePeriod", QString::number(stateSetting.updatePeriod));

    iniSetting.beginGroup("State");
    iniSetting.setValue("Details", stateSetting.details);
    iniSetting.setValue("State", stateSetting.state);
    iniSetting.setValue("StartTimestamp", QString::number(stateSetting.startTimeStamp));
    iniSetting.setValue("EndTimestamp", QString::number(stateSetting.endTimeStamp));
    iniSetting.endGroup();

    iniSetting.beginGroup("Images");
    iniSetting.setValue("LargeImage", stateSetting.largeImage);
    iniSetting.setValue("LargeTooltip", stateSetting.largeTooltip);
    iniSetting.setValue("SmallImage", stateSetting.smallImage);
    iniSetting.setValue("SmallTooltip", stateSetting.smallTooltip);

    iniSetting.sync();
}

DiscordController::DiscordController(QObject *parent): QThread(parent){
}

void DiscordController::run(){
    QDateTime currentTime = QDateTime::currentDateTime();
    startTime = currentTime.toMSecsSinceEpoch();
    StartCustomState();
}

void DiscordController::UpdateState(DiscordState *state, uint sleepMillSec){
    discord::Activity activity{};
    stateSetting = ReadSetting();
    activity.SetDetails(stateSetting.details.toStdString().c_str());
    activity.SetState(stateSetting.state.toStdString().c_str());
    activity.GetAssets().SetSmallImage(stateSetting.smallImage.toStdString().c_str());
    activity.GetAssets().SetSmallText(stateSetting.smallTooltip.toStdString().c_str());
    activity.GetAssets().SetLargeImage(stateSetting.largeImage.toStdString().c_str());
    activity.GetAssets().SetLargeText(stateSetting.largeTooltip.toStdString().c_str());
    if (stateSetting.startTimeStamp == 0) {
        activity.GetTimestamps().SetStart(startTime);
    }else{
        activity.GetTimestamps().SetStart(stateSetting.startTimeStamp);
    }
    activity.GetTimestamps().SetEnd(stateSetting.endTimeStamp);
    activity.SetType(discord::ActivityType::Playing);
    state->core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
    { qDebug() << ((result == discord::Result::Ok) ? "Succeeded" : "Failed")
               << " updating activity!\n"; });
    state->core->RunCallbacks();
    qDebug() << "sleepMillSec" << sleepMillSec;
    msleep(sleepMillSec);
}

// 設定自定義 discord 遊戲狀態
void DiscordController::StartCustomState(){
    stateSetting = ReadSetting();
    DiscordState state{};
    discord::Core* core{};
    auto result = discord::Core::Create(stateSetting.clientID, DiscordCreateFlags_Default, &core);

    state.core.reset(core);
    if (!state.core) {
        qDebug() << "Failed to instantiate discord core! (err " << static_cast<int>(result)
                 << ")\n";
        std::exit(-1);
    }

    std::signal(SIGINT, [](int)
    { interrupted = true;
        qDebug() << "Receive SIGINT! 再見";
    });

    // 狀態更新 loop
    do
    {
        UpdateState(&state, stateSetting.updatePeriod);
    } while (!interrupted);
}
